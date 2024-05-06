
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

const unsigned int ROWS = 3;
const unsigned int COLS = 3;


//using namespace std;

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

static void draw(int * position, std::vector<unsigned int> & indicies) {
    int r = position[0];
    int c  = position[1];
    //std::cout << "POSITION DATA" << std::endl;
    //std::cout << r << " " << c << std::endl;
    indicies.push_back((ROWS + 1) * r + c);
    indicies.push_back((ROWS + 1) * (r + 1) + c);
    indicies.push_back((ROWS + 1) * r + c + 1);

    indicies.push_back((ROWS + 1) * (r + 1) + c);
    indicies.push_back((ROWS + 1) * r + c + 1);
    indicies.push_back((ROWS + 1) * (r + 1) + c + 1);
}

static ShaderProgramSource ParseShader(const std::string & filepath) {
    std::ifstream stream(filepath);

    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while(getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                // vertex mode
                type = ShaderType::VERTEX;
            } else if (line.find("fragment") != std::string::npos) {
                // fragment mode
                type = ShaderType::FRAGMENT;
            }

        } else {
            // add to source code
            ss[(int)type] << line << '\n';
        }
    }

    return {ss[0].str(), ss[1].str()};
}

static unsigned int CompileShader(unsigned int type, const std::string & source) {
    unsigned int id = glCreateShader(type);

    // source must exist
    const char * src = source.c_str();

    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    // error handeling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char * message = (char *)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to Compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string & vertexShader, const std::string & fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
    
    
 
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1200, 980, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "glewInit Failed!\n";
    }

    /*float positions[] = {
        -1.0f, 1.0f,  // 0
        0.0f, 1.0f, // 1
        1.0f, 1.0f, // 2
        -1.0f, 0.0f, // 3
        0.0f, 0.0f, // 4
        1.0f, 0.0f, // 5
        -1.0f, -1.0f, // 6
        0.0f, -1.0f, // 7
        1.0f, -1.0f // 8
    };*/

    std::vector<float> positions;

    // populate position buffer
    for (int i = 0; i < ROWS + 1; i++) {
        for (int j = 0; j < COLS + 1; j++) {
            float xval = -1 + (2.0 / ROWS * j);
            float yval = 1 - (2.0 / COLS * i);
            positions.push_back(xval);
            positions.push_back(yval);
        }
    }


    // index buffer
    std::vector<unsigned int> indicies;

    // populate indicies
    int toDraw[][2] = {{0, 0}, {2, 0}, {1, 1}, {0, 2}, {2, 2}};
    int toDraw2[][2] = {{1, 0}, {0, 1}, {1, 2}, {2, 1}};


    for (int * position : toDraw2) {
        draw(position, indicies);
    }
 
     // length of arr should be (ROWS + 1)(COLS + 1)
    // make static position arr
    float newPositions[32];
    std::copy(positions.begin(), positions.end(), newPositions);

    // make static indicies arr
    unsigned int newIndicies[5 * 3 * 2];
    std::copy(indicies.begin(), indicies.end(), newIndicies);

    // print from new indicies
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(newPositions), newPositions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    // index buffer object
    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(newIndicies), newIndicies, GL_STATIC_DRAW);


    
    struct ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    
    //std::cout << source.VertexSource << std::endl;
    //std::cout << "VERTEX" << std::endl;
    //std::cout << source.FragmentSource << std::endl;
    //std::cout << "FRAGMENT" << std::endl;
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);


        // no index buffer
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        // uses index buffer
        glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, nullptr);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);



        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}