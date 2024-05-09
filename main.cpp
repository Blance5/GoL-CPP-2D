
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>


#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"

const unsigned int ROWS = 50;
const unsigned int COLS = 50;

static void draw(int x, int y, std::vector<unsigned int> & indicies) {
    //std::cout << "POSITION DATA" << std::endl;
    //std::cout << r << " " << c << std::endl;
    indicies.push_back((ROWS + 1) * x + y);
    indicies.push_back((ROWS + 1) * (x + 1) + y);
    indicies.push_back((ROWS + 1) * x + y + 1);

    indicies.push_back((ROWS + 1) * (x + 1) + y);
    indicies.push_back((ROWS + 1) * x + y + 1);
    indicies.push_back((ROWS + 1) * (x + 1) + y + 1);
}


struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};


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

static void genActiveGrid(std::vector<int> activeSquares, int (& activeGrid)[ROWS][COLS]) {
    for (int i = 0; i < activeSquares.size() - 1; i += 2) {
        activeGrid[activeSquares[i]][activeSquares[i + 1]] = 1;
    }
}

static void calculateNeighbors(std::vector<int> activeSquares, int (& neighborCountGrid)[ROWS][COLS]) {
    int activeGrid[ROWS][COLS] = {0};
    genActiveGrid(activeSquares, activeGrid);
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            for (int n = r - 1; n < r + 2; n++) {
                for (int m = c - 1; m < c + 2; m++) {
                    // if out of bounds, or if the square is the current square, or if the square is not active
                    if (n < 0 || m < 0 || n >= ROWS || m >= COLS || (n == r && m == c) || activeGrid[n][m] == 0) {
                        continue;
                    }
                    neighborCountGrid[r][c]++;
                }
            }
        }
    }
}

static void updateSquares(std::vector<int> & activeSquares) {
    if (activeSquares.empty()) {
        return;
    }

    int neighborCountGrid[ROWS][COLS] = {0};
    calculateNeighbors(activeSquares, neighborCountGrid);

    int activeGrid[ROWS][COLS] = {0};
    genActiveGrid(activeSquares, activeGrid);

    // update activeGrid
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if ((neighborCountGrid[i][j] < 2 || neighborCountGrid[i][j] > 3) && activeGrid[i][j] == 1) {
                activeGrid[i][j] = 0;
            } else if (neighborCountGrid[i][j] == 3 && activeGrid[i][j] == 0) {
                activeGrid[i][j] = 1;
            }
        }
    }

    /*
    // printing activeGrid
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            std::cout << activeGrid[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl << std::endl;
    */


    // update activeSquares
    activeSquares.clear();
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (activeGrid[i][j] == 1) {
                activeSquares.push_back(i);
                activeSquares.push_back(j);
            }
        }
    }
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

    std::srand(std::time(nullptr)); // use current time as seed for random generator
    

    // populate indicies
    // IN FORM x, y, x, y, x, y
    std::vector<int> activeSquares;

    int random_value = std::rand();
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (random_value % 4 == 0) {
                activeSquares.push_back(i);
                activeSquares.push_back(j);
            }
            random_value = std::rand();
        }
    }


    

    float newPositions[(COLS + 1) * (ROWS + 1) * 2];
    std::copy(positions.begin(), positions.end(), newPositions);



    VertexArray va;
    VertexBuffer vb(newPositions, sizeof(newPositions));

    VertexBufferLayout layout;
    layout.Push(2, GL_FLOAT);
    va.AddBuffer(vb, layout);

    unsigned int newIndicies[ROWS * COLS * 6];

    


    
    struct ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    
    //std::cout << source.VertexSource << std::endl;
    //std::cout << "VERTEX" << std::endl;
    //std::cout << source.FragmentSource << std::endl;
    //std::cout << "FRAGMENT" << std::endl;
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    GLCall(int location = glGetUniformLocation(shader, "u_Color"));
    ASSERT(location != -1);


    GLCall(glBindVertexArray(0));
    GLCall(glUseProgram(0));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));



    float r = 0.0f;
    float increment = 0.05f;
    /* Loop until the user closes the window */
    int drawRate = 1;
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        GLCall(glUseProgram(shader));
        GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));

        /*GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
        GLCall(glEnableVertexAttribArray(0));
        GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));*/



        if (drawRate % 20 == 0) {
            updateSquares(activeSquares);
        }

        indicies.clear();
        if (!activeSquares.empty()) {
            
            for (int i = 0; i < activeSquares.size() - 1; i += 2) {
                draw(activeSquares[i], activeSquares[i + 1], indicies);
            }
        }
        // 6 = verticies per 1 square
            


        
        std::copy(indicies.begin(), indicies.end(), newIndicies);

        // index buffer object
        IndexBuffer ib(newIndicies, sizeof(unsigned int) * indicies.size());    

        va.Bind();
        // not neccesarry, vao binds buffer and ibo
        ib.Bind();
        

        // no index buffer
        //glDrawArrays(GL_TRIANGLES, 0, 6);


        // CALCULATE NEXT STATE OF INDICIES
        // 1. make 2d array activeSquares that contains coordinates ex {0, 0, 2, 1, 4, 5}
        // 2. call draw with indicies and activesquares
        // 3. draw from indicies
        // 4. calculate new activeSquares








        // uses index buffer
        GLCall(glDrawElements(GL_TRIANGLES, indicies.size(), GL_UNSIGNED_INT, nullptr));
        if (r > 1.0f) {
            increment = -0.003f;
        } else if (r < 0.0f) {
            increment = 0.003f;
        }
        r += increment;

        drawRate++;


        // calculate new activeSquares
        //updateSquares(& activeSquares[0][0]);




        /* Swap front and back buffers */
        glfwSwapBuffers(window);



        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}