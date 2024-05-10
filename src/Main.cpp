#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>


#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit()) {
        return -1;
    }
    
    GLCall(glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3));
    GLCall(glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3));
    GLCall(glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE));
    
 
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1200, 980, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    GLCall(glfwMakeContextCurrent(window));

    GLCall(glfwSwapInterval(1));

    if (glewInit() != GLEW_OK) {
        std::cout << "glewInit Failed!\n";
    }

    float positions[] = {
        -0.5f, -0.5f, // 0
        0.5f, -0.5f, // 1
         0.5f, 0.5f, // 2
        -0.5f, 0.5f, // 3
    };

    // index buffer
    unsigned int indicies[] {
        0, 1, 2,
        2, 3, 0
    };

    VertexArray va;
    VertexBuffer vb(positions, sizeof(positions));


    //VA 
    VertexBufferLayout layout;
    layout.Push(2, GL_FLOAT);
    va.AddBuffer(vb, layout);


    // index buffer object
    IndexBuffer ib(indicies, sizeof(indicies));


    Shader shader("res/shaders/Basic.shader");
    shader.Bind();
    shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);
    shader.Unbind();
    
    vb.Unbind();
    ib.Unbind();


    float r = 0.0f;
    float increment = 0.05f;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        shader.Bind();
        shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

        /*GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
        GLCall(glEnableVertexAttribArray(0));
        GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));*/


        va.Bind();
        // not neccesarry, vao binds buffer and ibo
        ib.Bind();


        // no index buffer
        //glDrawArrays(GL_TRIANGLES, 0, 6);


        // uses index buffer
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
        if (r > 1.0f) {
            increment = -0.003f;
        } else if (r < 0.0f) {
            increment = 0.003f;
        }
        r += increment;

        /* Swap front and back buffers */
        GLCall(glfwSwapBuffers(window));



        /* Poll for and process events */
        GLCall(glfwPollEvents());
    }

    glfwTerminate();
    return 0;
}