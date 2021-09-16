#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <GLFW/glfw3.h>

#include <array>
#include <hitobject/parse_hitobject.h>
#include <iostream>
#include <osu_reader/sliderpath.h>
#include <osu_reader/string_stuff.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const constexpr auto SCR_WIDTH = 600;
const constexpr auto SCR_HEIGHT = 600;

const constexpr auto log_size = 512;

constexpr const char* vertex_shader_source = R"GLSL(
#version 330 core
in vec2 position;
in vec2 vLocalPos;

out vec2 TexCoord;

uniform mat4 transform;

void main()
{
    gl_Position = transform * vec4(position, 0.5, 1.0);
    TexCoord = vLocalPos;
}
)GLSL";

constexpr const char* fragment_shader_source = R"GLSL(
#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform vec4 color;

void main(void)
{
    float dist = length(TexCoord);

    if(dist > 1) discard;
    FragColor = 2 * color / 3 + dist * color / 3;
}
)GLSL";

struct VAO {
    unsigned int vbo = 0, vao = 0;
    VAO() = default;
    VAO(VAO&& other)
    noexcept : vbo(other.vbo), vao(other.vao)
    {
        other.vbo = 0;
        other.vao = 0;
    }
    VAO& operator=(VAO&& other) noexcept
    {
        vbo = other.vbo;
        vao = other.vao;
        other.vbo = 0;
        other.vao = 0;
        return *this;
    }
    ~VAO()
    {
        if(vbo != 0) glDeleteBuffers(1, &vbo);
        if(vao != 0) glDeleteVertexArrays(1, &vao);
    }
};

auto point_vertices(const osu::Vector2& p, float radius)
{
    return std::array{
            p.x - radius, p.y + radius, -1.f, 1.f, // Top left
            p.x - radius, p.y - radius, -1.f, -1.f,// bottom left
            p.x + radius, p.y + radius, 1.f, 1.f,  // top right

            p.x - radius, p.y - radius, -1.f, -1.f,// bottom left
            p.x + radius, p.y + radius, 1.f, 1.f,  // top right
            p.x + radius, p.y - radius, 1.f, -1.f, // bottom right
    };
}

auto verts_to_vao(const std::vector<float>& verts)
{
    VAO instance{};

    glGenVertexArrays(1, &instance.vao);
    glGenBuffers(1, &instance.vbo);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(instance.vao);

    glBindBuffer(GL_ARRAY_BUFFER, instance.vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<int>(verts.size() * sizeof(verts.front())), verts.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    return instance;
}

auto generate_data(const osu::Slider& slider)
{
    constexpr const auto curve_radius = 0.75f;
    constexpr const auto control_radius = curve_radius * 0.6f;
    struct Data {
        VAO control_vao, curve_vao;
        int count_control, count_curve;
    };

    Data data{};

    // Functor to handle control and curve points
    const auto handle_point = [](auto output, auto radius) {
        return [radius, output](const auto& p) {
            const auto verts = point_vertices(p, radius);
            std::copy(verts.cbegin(), verts.cend(), output);
        };
    };

    // Control points
    std::vector<float> control_vertices;
    for(const auto& segment : slider.segments) {
        std::for_each(segment.points.cbegin(), segment.points.cend(), handle_point(std::back_inserter(control_vertices), control_radius));
    }

    data.count_control = static_cast<int>(control_vertices.size());
    data.control_vao = verts_to_vao(control_vertices);

    // Curve
    std::vector<float> curve_vertices;
    std::for_each(slider.points.cbegin(), slider.points.cend(), handle_point(std::back_inserter(curve_vertices), curve_radius));

    data.count_curve = static_cast<int>(curve_vertices.size());
    data.curve_vao = verts_to_vao(curve_vertices);

    return data;
}

auto get_slider(std::string_view slider_string)
{
    const auto slider_tokens = osu::split(slider_string, ',');
    auto slider = parse_slider(slider_tokens).value();

    if(slider.segments.empty() || slider.segments.front().points.empty()) throw std::runtime_error{"Empty slider"};

    slider.points = sliderpath(slider);
    slider.distances = pathlengths(slider.points);

    return slider;
}

auto get_slider_bounds(const osu::Slider& slider)
{
    struct Bounds {
        float min_x, min_y, max_x, max_y;
    };
    Bounds bounds{};
    bounds.min_x = bounds.max_x = slider.segments.front().points.front().x;
    bounds.min_y = bounds.max_y = slider.segments.front().points.front().y;
    for(const auto& p : slider.points) {
        if(p.x < bounds.min_x) bounds.min_x = p.x;
        if(p.x > bounds.max_x) bounds.max_x = p.x;
        if(p.y < bounds.min_y) bounds.min_y = p.y;
        if(p.y > bounds.max_y) bounds.max_y = p.y;
    }
    for(const auto& segment : slider.segments) {
        for(const auto& p : segment.points) {
            if(p.x < bounds.min_x) bounds.min_x = p.x;
            if(p.x > bounds.max_x) bounds.max_x = p.x;
            if(p.y < bounds.min_y) bounds.min_y = p.y;
            if(p.y > bounds.max_y) bounds.max_y = p.y;
        }
    }

    bounds.min_x -= 1;
    bounds.max_x += 1;
    bounds.min_y -= 1;
    bounds.max_y += 1;

    // Make bounds square and fix proportions
    const auto [min_dist, max_dist] = std::minmax(bounds.max_x - bounds.min_x, bounds.max_y - bounds.min_y);
    const auto missing = (max_dist - min_dist);
    if(min_dist == bounds.max_x - bounds.min_x) {
        bounds.max_x += missing / 2;
        bounds.min_x -= missing / 2;
    } else if(min_dist == bounds.max_y - bounds.min_y) {
        bounds.max_y += missing / 2;
        bounds.min_y -= missing / 2;
    }
    return bounds;
}

auto update_slider(osu::Slider& slider)
{
    slider.points = sliderpath(slider);
    slider.distances = pathlengths(slider.points);
    osu::fix_slider_length(slider);
}


constexpr const auto tests_data = std::array{
        std::pair{"catmull", "272,71,214160,6,0,C|243:81|165:10|116:24,1,192,14|0,0:1|0:2,0:0:0:0:"},
        std::pair{"bezier", "45,213,28773,6,0,B|144:152|208:265|208:265|265:163|379:202,1,389.999971389771"},
        std::pair{"bezier2", "79,170,28773,6,0,B|283:15|213:136|354:159|155:308|371:310,1,454.999966621399"},
        std::pair{"bezier-butterfly", "256,192,74363,118,0,B|208:4|8:8|8:8|40:36|48:63|48:63|44:104|44:104|92:128|76:188|76:188|112:204|152:192|152:192|56:248|32:360|32:360|64:332|100:332|100:332|152:348|196:320|196:320|216:280|256:276|256:276|261:255|261:255|254:246|254:246|259:238|259:238|251:236|251:236|263:225|263:225|253:214|253:214|262:205|262:205|256:201|256:201|256:160,1,1200.0479469394"},
        std::pair{"perfect", "157,55,99770,2,0,P|182:65|209:67,1,55.2450004636345,8|0,0:0|0:0,0:0:0:0:"},
        std::pair{"perfect2", "255,163,1435,2,0,P|366:173|449:259,2,222.74999320221,2|2|2,0:0|0:0|0:0,0:0:0:0:"},
        std::pair{"perfect3", "275,350,2666,6,0,P|221:355|150:60,1,446.250017023087,2|0,0:0|0:0,0:0:0:0:"},
        std::pair{"perfect4", "44,198,28773,6,0,P|173:314|112:275,1,1852.49986410141"},
};

int main(int arc, char* argv[])
{
    if(arc < 2 || std::strcmp(argv[1], "-h") == 0 || std::strcmp(argv[1], "--help") == 0) {
        std::cout << "Pass in the name of the test to use\n";
        for([[maybe_unused]] const auto [test, test_data] : tests_data) {
            std::cout << "\t- " << test << '\n';
        }
        std::cout << "Once test is running, visualise point order by pressing up and down arrows\n";
        return 0;
    }
    const char* slider_string = argv[1];
    auto test_found = false;
    for(const auto [text, data] : tests_data) {
        if(strcmp(text, slider_string) == 0) {
            slider_string = data;
            std::cout << "Using test " << text << '\n';
            test_found = true;
            break;
        }
    }
    if(!test_found) std::cout << "No test with that name found. Attempting to treat input as slider...\n";

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if(window == nullptr) {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if(gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) == 0) {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    // build and compile our shader program

    // vertex shader
    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
    glCompileShader(vertex_shader);
    // check for shader compile errors
    int success = 0;
    std::array<char, log_size> info_log{};
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if(success == 0) {
        glGetShaderInfoLog(vertex_shader, info_log.size(), nullptr, info_log.data());
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << info_log.data() << '\n';
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment_shader_source, nullptr);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(success == 0) {
        glGetShaderInfoLog(fragmentShader, info_log.size(), nullptr, info_log.data());
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << info_log.data() << '\n';
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertex_shader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(success == 0) {
        glGetProgramInfoLog(shaderProgram, info_log.size(), nullptr, info_log.data());
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << info_log.data() << '\n';
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragmentShader);

    glUseProgram(shaderProgram);

    constexpr const std::array<float, 4> control_color = {1, 0, 0, 1};
    constexpr const std::array<float, 4> curve_color = {0, 0, 1, 1};

    const auto set_color = [&shaderProgram](const auto color) {
        static const auto loc = glGetUniformLocation(shaderProgram, "color");
        glUniform4f(loc, color[0], color[1], color[2], color[3]);
    };

    auto slider = get_slider(slider_string);
    const auto orig_length = slider.length;

    auto update_size = 0.f;
    auto last_time = glfwGetTime();
    const auto max_size = std::max(pathlengths(sliderpath(slider)).back(), slider.length);

    // Get max boundaries for field and set ortho matrix
    slider.length = max_size;
    update_slider(slider);
    const auto bounds = get_slider_bounds(slider);
    glm::mat4 projection = glm::ortho(bounds.min_x, bounds.max_x, bounds.max_y, bounds.min_y);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(projection));

    slider.length = orig_length;
    update_slider(slider);

    while(glfwWindowShouldClose(window) == 0) {
        processInput(window);

        const auto frame_time = glfwGetTime() - last_time;
        last_time = glfwGetTime();

        if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) update_size += static_cast<float>(100. * frame_time);
        if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) { update_size -= static_cast<float>(100. * frame_time); }

        slider.length += static_cast<float>(update_size * frame_time);
        if(slider.length > max_size || slider.length < 0) update_size = -update_size;

        update_slider(slider);
        const auto data = generate_data(slider);

        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        set_color(curve_color);
        glBindVertexArray(data.curve_vao.vao);
        glDrawArrays(GL_TRIANGLES, 0, data.count_curve);

        set_color(control_color);
        glBindVertexArray(data.control_vao.vao);
        glDrawArrays(GL_TRIANGLES, 0, data.count_control);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
}

void framebuffer_size_callback(GLFWwindow* /*window*/, int width, int height)
{
    glViewport(0, 0, width, height);
}
