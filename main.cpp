#include <iostream>
#include <vector>

#include "glm/glm.hpp"
#include "glm/common.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "opengl_utilities.h"
#include "mesh_generation.h"

/* Keep the global state inside this struct */
static struct {
	glm::dvec2 mouse_position;
	glm::ivec2 screen_dimensions = glm::ivec2(960, 960);
    GLuint scene;
    GLuint program;
    glm::dvec3 shape_color= glm::dvec3(1.,1.,1.);
    GLuint shininess=32;
} Globals;

/* GLFW Callback functions */
static void ErrorCallback(int error, const char* description)
{
	std::cerr << "Error: " << description << std::endl;
}

static void CursorPositionCallback(GLFWwindow* window, double x, double y)
{
	Globals.mouse_position.x = x;
	Globals.mouse_position.y = y;
}

static void WindowSizeCallback(GLFWwindow* window, int width, int height)
{
	Globals.screen_dimensions.x = width;
	Globals.screen_dimensions.y = height;

	glViewport(0, 0, width, height);
}

static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods){
    std::cout << key <<std::endl;
        // Q
        if (key== 81) {
            Globals.scene = 1;
        }
        // W
        else if (key== 87) {
            Globals.scene = 2;
        }
        // E
        else if (key== 69) {
            Globals.scene = 3;
        }
        // R
        else if (key== 82) {
            Globals.scene = 4;
        }
        // T
        else if (key== 84) {
            Globals.scene = 5;
        }
        // Y
        else if (key== 89) {
            Globals.scene = 6;
        }
        else{
            Globals.scene = 0;
        }
}



int main(int argc, char* argv[])
{
	/* Set GLFW error callback */
	glfwSetErrorCallback(ErrorCallback);

	/* Initialize the library */
	if (!glfwInit())
	{
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	/* Create a windowed mode window and its OpenGL context */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow* window = glfwCreateWindow(
		Globals.screen_dimensions.x, Globals.screen_dimensions.y,
		"Begum Celik", NULL, NULL
	);
	if (!window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	/* Move window to a certain position [do not change] */
	glfwSetWindowPos(window, 10, 50);
	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	/* Enable VSync */
	glfwSwapInterval(1);
    /* Enable Keyboard Control */
    glfwSetKeyCallback(window, KeyCallback);
    
	/* Load OpenGL extensions with GLAD */
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}
    
	/* Set GLFW Callbacks */
	glfwSetCursorPosCallback(window, CursorPositionCallback);
	glfwSetWindowSizeCallback(window, WindowSizeCallback); // for resizable content

	/* Configure OpenGL */
	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);

	/* Creating Meshes */
    
    // Sphere Mesh
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<GLuint> indices;
    int vertical_segment =16, rotation_segment=16;
    
    GenerateParametricShapeFrom2D(positions, normals, indices, ParametricHalfCircle, vertical_segment, rotation_segment);
    VAO sphereVAO(positions, normals, indices);
    
    // Torus Mesh
    positions.clear();
    normals.clear();
    indices.clear();
    
    GenerateParametricShapeFrom2D(positions, normals, indices, ParametricCircle, vertical_segment, rotation_segment);
    VAO torusVAO(positions, normals, indices);
    
    // Spikes Torus Mesh
    positions.clear();
    normals.clear();
    indices.clear();
    
    GenerateParametricShapeFrom2D(positions, normals, indices, ParametricSpikes, 100, 100);
    VAO spikestorusVAO(positions, normals, indices);
    
    // Spikes Mesh
    positions.clear();
    normals.clear();
    indices.clear();
    
    GenerateParametricShapeFrom2D_2(positions, normals, indices, ParametricSpikes, 100, 100);
    VAO spikesVAO(positions, normals, indices);

	/* Creating Programs */
	GLuint program1 = CreateProgramFromSources(
		R"VERTEX(
            #version 330 core

            layout(location = 0) in vec3 a_position;
                                               
            uniform mat4 u_transform;
                                               
            void main()
            {
                gl_Position = u_transform *  vec4(a_position, 1);
            }
		)VERTEX",

		R"FRAGMENT(
            #version 330 core
            
            
            out vec4 out_color;

            void main()
            {
                out_color = vec4(1,1,1,1); // color of wireframe
            }
		)FRAGMENT");
    
    GLuint program2 = CreateProgramFromSources(
        R"VERTEX(
            #version 330 core

            layout(location = 0) in vec3 a_position;
            layout(location = 1) in vec3 a_normal;
                                               
            uniform mat4 u_transform;


            out vec3 vertex_position;
            out vec3 vertex_normal;
            void main()
            {
                gl_Position = u_transform *  vec4(a_position, 1);
                vertex_normal = vec3(u_transform * vec4(a_normal,0));
                vertex_position = vec3(gl_Position);
            }
        )VERTEX",

        R"FRAGMENT(
            #version 330 core
            
            in vec3 vertex_position;
            in vec3 vertex_normal;
            
            out vec4 out_color;

            void main()
            {
                vec3 color= vec3(0);
                                               
                vec3 surface_position = vertex_position;
                vec3 surface_normal = normalize(vertex_normal);
           
                out_color = vec4(surface_normal, 1); // normal vectors as color values
            }
        )FRAGMENT");
    
    GLuint program3 = CreateProgramFromSources(
        R"VERTEX(
            #version 330 core

            layout(location = 0) in vec3 a_position;
            layout(location = 1) in vec3 a_normal;
                                               
            uniform mat4 u_transform;


            out vec3 vertex_position;
            out vec3 vertex_normal;
            void main()
            {
                gl_Position = u_transform *  vec4(a_position, 1);
                vertex_normal = vec3(u_transform * vec4(a_normal,0));
                vertex_position = vec3(gl_Position);
            }
        )VERTEX",

        R"FRAGMENT(
            #version 330 core
                                                                  
            in vec3 vertex_position;
            in vec3 vertex_normal;
            
            out vec4 out_color;

            void main()
            {
                vec3 color= vec3(0);
                                               
                vec3 surface_color = vec3(0.5, 0.5, 0.5); // gray surface color
                vec3 surface_position = vertex_position;
                vec3 surface_normal = normalize(vertex_normal);
                            
                // ambient light
                float ambient_k = 1;
                vec3 ambient_color= vec3(0.5, 0.5 , 0.5 );
                color += ambient_k * ambient_color * surface_color;
                
                // directional light
                vec3 light_direction = normalize(vec3(1, 1, -1)); // direction of light from the left upper corner to origin
                vec3 light_color= vec3(0.4, 0.4, 0.4); // color of the directional light
                
                float diffuse_k= 1;
                float diffuse_intensity = max(0, dot(light_direction, surface_normal));
                color += diffuse_k * diffuse_intensity * light_color * surface_color;
                 
                vec3 view_dir = vec3(0,0,-1);
                vec3 reflect_dir = reflect(-light_direction, surface_normal);
                vec3 halfway_dir = normalize(view_dir + light_direction);
                float specular_k = 1;
                int shininess= 64;
                float specular_intensity= pow(max(0, dot(halfway_dir, surface_normal)), shininess);
                color += specular_k * specular_intensity * light_color;
                                               
                out_color = vec4(color, 1);
            }
        )FRAGMENT");
    
    GLuint program4 = CreateProgramFromSources(
        R"VERTEX(
            #version 330 core

            layout(location = 0) in vec3 a_position;
            layout(location = 1) in vec3 a_normal;

            uniform mat4 u_transform;
            
            out vec3 vertex_position;
            out vec3 vertex_normal;
            void main()
            {
                gl_Position = u_transform *  vec4(a_position, 1);
                vertex_normal = vec3(u_transform * vec4(a_normal,0));
                vertex_position = vec3(gl_Position);
            }
        )VERTEX",

        R"FRAGMENT(
            #version 330 core
                                               
            uniform vec2 u_mouse_position;
            uniform vec3 u_color;
            uniform int u_shininess;
                                               
            in vec3 vertex_position;
            in vec3 vertex_normal;
            
            out vec4 out_color;

            void main()
            {
                vec3 color= vec3(0);
                                               
                vec3 surface_color = u_color;
                vec3 surface_position = vertex_position;
                vec3 surface_normal = normalize(vertex_normal);
                                               
                float ambient_k = 1;
                vec3 ambient_color= vec3(0.5, 0.5 , 0.5);
                color += ambient_k * ambient_color * surface_color;
                
                vec3 light_direction = normalize(vec3(1,1, -1));
                vec3 light_color= vec3(0.4, 0.4, 0.4);
                                               
                float diffuse_k= 1;
                float diffuse_intensity = max(0, dot(light_direction, surface_normal));
                color += diffuse_k * diffuse_intensity * light_color * surface_color;
                 
                vec3 view_dir = vec3(0,0,-1);
                vec3 halfway_dir = normalize(view_dir + light_direction);
                float specular_k = 1;
                int shininess= u_shininess;
                float specular_intensity= pow(max(0, dot(halfway_dir, surface_normal)), shininess);
                color += specular_k * specular_intensity * light_color;
                   
                                               
                // point light
                vec3 point_light_position = (vec3(u_mouse_position, -1));
                vec3 point_light_color= vec3(0.5, 0.5, 0.5);
                vec3 to_point_light = normalize(point_light_position - surface_position);
                
                diffuse_k= 1;
                diffuse_intensity = max(0, dot(to_point_light, surface_normal));
                color += diffuse_k * diffuse_intensity * point_light_color * surface_color;
                                                
                halfway_dir = normalize(view_dir + to_point_light);
                specular_k = 1;
                specular_intensity= pow(max(0, dot(halfway_dir, surface_normal)), shininess);
                color += specular_k * specular_intensity * point_light_color;
                                               
                                               
                out_color = vec4(color, 1);
            }
        )FRAGMENT");
    
    GLuint program5 = CreateProgramFromSources(
        R"VERTEX(
            #version 330 core

            layout(location = 0) in vec3 a_position;
            layout(location = 1) in vec3 a_normal;

            uniform mat4 u_transform;

            out vec3 vertex_position;
            out vec3 vertex_normal;
            void main()
            {
                gl_Position = u_transform *  vec4(a_position, 1);
                vertex_normal = vec3(u_transform * vec4(a_normal,0));
                vertex_position = vec3(gl_Position);
            }
        )VERTEX",

        R"FRAGMENT(
            #version 330 core
                                               
            uniform vec2 u_mouse_position;
           
            in vec3 vertex_position;
            in vec3 vertex_normal;
            uniform vec3 u_color;
            
            out vec4 out_color;

            void main()
            {
                vec3 color= vec3(0);
                                               
                vec3 surface_color = u_color;
                vec3 surface_position = vertex_position;
                vec3 surface_normal = normalize(vertex_normal);
                                               
                float ambient_k = 1;
                vec3 ambient_color= vec3(0.5, 0.5 , 0.5);
                color += ambient_k * ambient_color * surface_color;
                
                vec3 light_direction = normalize(vec3(1,1, -1));
                vec3 light_color= vec3(0.4, 0.4, 0.4);
                                               
                float diffuse_k= 1;
                float diffuse_intensity = max(0, dot(light_direction, surface_normal));
                color += diffuse_k * diffuse_intensity * light_color * surface_color;
                 
                vec3 view_dir = vec3(0,0,-1);
                vec3 halfway_dir = normalize(view_dir + light_direction);
                float specular_k = 1;
                int shininess= 128;
                float specular_intensity= pow(max(0, dot(halfway_dir, surface_normal)), shininess);
                color += specular_k * specular_intensity * light_color;
                   
                                               
                // point light
                vec3 point_light_position = (vec3(u_mouse_position, -1));
                vec3 point_light_color= vec3(0.5, 0.5, 0.5);
                vec3 to_point_light = normalize(point_light_position - surface_position);
                
                diffuse_k= 1;
                diffuse_intensity = max(0, dot(to_point_light, surface_normal));
                color += diffuse_k * diffuse_intensity * point_light_color * surface_color;
                                                
                halfway_dir = normalize(view_dir + to_point_light);
                specular_k = 1;
                specular_intensity= pow(max(0, dot(halfway_dir, surface_normal)), shininess);
                color += specular_k * specular_intensity * point_light_color;
                                               
                                               
                out_color = vec4(color, 1);
            }
        )FRAGMENT");
    
    GLuint program6 = CreateProgramFromSources(
        R"VERTEX(
            #version 330 core

            layout(location = 0) in vec3 a_position;
            layout(location = 1) in vec3 a_normal;
                                               
            uniform mat4 u_transform;


            out vec3 vertex_position;
            out vec3 vertex_normal;
            void main()
            {
                gl_Position = u_transform *  vec4(a_position, 1);
                vertex_normal = vec3(u_transform * vec4(a_normal,0));
                vertex_position = vec3(gl_Position);
            }
        )VERTEX",

        R"FRAGMENT(
            #version 330 core
                                                                  
            in vec3 vertex_position;
            in vec3 vertex_normal;
            uniform vec3 u_color;
            out vec4 out_color;

            void main()
            {
                vec3 color= vec3(0);
                                               
                vec3 surface_color =  u_color;
                vec3 surface_position = vertex_position;
                vec3 surface_normal = normalize(vertex_normal);
                            
                // ambient light
                float ambient_k = 1;
                vec3 ambient_color= vec3(0.5, 0.5 , 0.5 );
                color += ambient_k * ambient_color * surface_color;
                
                // directional light
                vec3 light_direction = normalize(vec3(1, 1, -1)); // direction of light from the left upper corner to origin
                vec3 light_color= vec3(0.4, 0.4, 0.4); // color of the directional light
                
                float diffuse_k= 1;
                float diffuse_intensity = max(0, dot(light_direction, surface_normal));
                color += diffuse_k * diffuse_intensity * light_color * surface_color;
                 
                vec3 view_dir = vec3(0,0,-1);
                vec3 reflect_dir = reflect(-light_direction, surface_normal);
                vec3 halfway_dir = normalize(view_dir + light_direction);
                float specular_k = 1;
                int shininess= 64;
                float specular_intensity= pow(max(0, dot(halfway_dir, surface_normal)), shininess);
                color += specular_k * specular_intensity * light_color;
                                               
                out_color = vec4(color, 1);
            }
        )FRAGMENT");
    
   
    
	if (program1 == NULL && program2 == NULL && program3 == NULL && program4 == NULL && program5 == NULL && program6 == NULL)
	{
		glfwTerminate();
		return -1;
	}

    
	/* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        /* Uniform Values */
        auto u_mouse_position_location = glGetUniformLocation(Globals.program, "u_mouse_position");
        auto u_color_location = glGetUniformLocation(Globals.program, "u_color");
        auto u_transform_location = glGetUniformLocation(Globals.program, "u_transform");
        auto u_shininess_location = glGetUniformLocation(Globals.program, "u_shininess");
        
        /* Scenes */
        Globals.program= 0;
        if (Globals.scene == 1) {
            Globals.program = program1;
        }
        else if (Globals.scene == 2) {
            Globals.program = program2;
        }
        else if (Globals.scene == 3) {
            Globals.program = program3;
        }
        else if (Globals.scene == 4) {
            Globals.program = program4;
        }
        else if (Globals.scene == 5) {
            Globals.program = program5;
        }
        else if (Globals.scene == 6) {
            Globals.program = program6;
        }
        glUseProgram(Globals.program);
        
        /* Dynamic Change in Mouse Positions */
        auto mouse_positions = Globals.mouse_position / glm::dvec2(Globals.screen_dimensions);
        mouse_positions.y = 1. - mouse_positions.y;
        mouse_positions = mouse_positions * 2. - 1. ;
        glUniform2fv(u_mouse_position_location, 1, glm::value_ptr(glm::vec2(mouse_positions)));
    
        // Sphere Transformation
        glm::mat4 transform(1.0);
        transform = glm::translate(transform, glm::vec3(-0.5, 0.5, 0));
        transform = glm::scale(transform,glm::vec3(0.4));
        transform = glm::rotate(transform, glm::radians(float(glfwGetTime()) * 10), glm::vec3(1,1,0));
        glUniformMatrix4fv(u_transform_location, 1, GL_FALSE, glm::value_ptr(transform));

        // Sphere WireFrame
        glBindVertexArray(sphereVAO.id);
        if(Globals.program == program1)
            glDrawElements(GL_LINE_STRIP, sphereVAO.element_array_count, GL_UNSIGNED_INT, 0);
      
        else if(Globals.program == program2 | Globals.program == program3)
            glDrawElements(GL_TRIANGLES, sphereVAO.element_array_count, GL_UNSIGNED_INT, 0);
       
        else if(Globals.program == program4){
            // Sphere Color
            glUniform3fv(u_color_location,1,glm::value_ptr(glm::vec3(0.5,0.5,0.5)));
            // Sphere Shininess
            glUniform1i(u_shininess_location,128);
            glDrawElements(GL_TRIANGLES, sphereVAO.element_array_count, GL_UNSIGNED_INT, 0);
        }
        
        else if(Globals.program == program5){

            // Chasing Sphere
            glUniform3fv(u_color_location,1,glm::value_ptr(glm::vec3(0.5,0.5,0.5)));
            glm::vec2 chasing_pos = glm::mix(glm::vec2(mouse_positions), chasing_pos, 0.99);
            transform = glm::mat4(1.0);
            transform = glm::translate(transform, glm::vec3(chasing_pos, 0));
            transform = glm::scale(transform,glm::vec3(0.3));
            
            glUniformMatrix4fv(u_transform_location, 1, GL_FALSE, glm::value_ptr(transform));
            glDrawElements(GL_TRIANGLES, sphereVAO.element_array_count, GL_UNSIGNED_INT, 0);
            
            if (glm::distance(glm::vec2(mouse_positions) , chasing_pos) > 0.3*2)
                glUniform3fv(u_color_location,1,glm::value_ptr(glm::vec3(0,1,0)));
            
            else
            glUniform3fv(u_color_location,1,glm::value_ptr(glm::vec3(1,0,0)));
            
            transform = glm::mat4(1.0);
            transform = glm::translate(transform, glm::vec3(mouse_positions, 0));
            transform = glm::scale(transform,glm::vec3(0.3));
            
            glUniformMatrix4fv(u_transform_location, 1, GL_FALSE, glm::value_ptr(transform));
            glDrawElements(GL_TRIANGLES, sphereVAO.element_array_count, GL_UNSIGNED_INT, 0);
        }
        
        else{
            // Scene 6
            transform = glm::mat4(1.0);
            transform = glm::scale(transform,glm::vec3(1.2));
            transform = glm::rotate(transform, 90.0f, glm::vec3(1,0,0));
            
            
            std::vector<glm::vec3> p2;
            for (int j=0; j<positions.size(); j++) {
                auto p= transform * glm::vec4(positions[j],1);
                
                p2.push_back(glm::vec3(p));
            }

            for (GLuint i=0; i < p2.size(); i += 1) {
                transform = glm::mat4(1.0);
                transform = glm::translate(transform, glm::vec3(p2[i]));
                transform = glm::scale(transform,glm::vec3(0.05));
                transform = glm::rotate(transform, glm::radians(float(glfwGetTime()) * 30), glm::vec3(1,1,0));
                glUniformMatrix4fv(u_transform_location, 1, GL_FALSE, glm::value_ptr(transform));
                glUniform3fv(u_color_location,1,glm::value_ptr(p2[i]+glm::vec3(0.3,0.3,0.3)));
                
                // Torus WireFrame
                glBindVertexArray(torusVAO.id);
                glDrawElements(GL_TRIANGLES, torusVAO.element_array_count, GL_UNSIGNED_INT, 0);
            }
            
        }
        
        
        
        // Torus Transformation
        transform = glm::mat4(1.0);
        transform = glm::translate(transform, glm::vec3(0.5, 0.5, 0));
        transform = glm::scale(transform,glm::vec3(0.4));
        transform = glm::rotate(transform, glm::radians(float(glfwGetTime()) * 10), glm::vec3(1,1,0));
        glUniformMatrix4fv(u_transform_location, 1, GL_FALSE, glm::value_ptr(transform));

        // Torus WireFrame
        glBindVertexArray(torusVAO.id);
        if(Globals.program == program1){
            glDrawElements(GL_LINE_STRIP, torusVAO.element_array_count, GL_UNSIGNED_INT, 0);}
        
        else if(Globals.program == program2 | Globals.program == program3)
            glDrawElements(GL_TRIANGLES, torusVAO.element_array_count, GL_UNSIGNED_INT, 0);
        
        else if(Globals.program == program4){
            // Torus Color
            glUniform3fv(u_color_location,1,glm::value_ptr(glm::vec3(1,0,0)));
            // Torus Shininess
            glUniform1i(u_shininess_location,32);
            glDrawElements(GL_TRIANGLES, torusVAO.element_array_count, GL_UNSIGNED_INT, 0);
        }
       
        
        
        // Spikes Torus Transformation
        transform = glm::mat4(1.0);
        transform = glm::translate(transform, glm::vec3(-0.5, -0.5, 0));
        transform = glm::scale(transform,glm::vec3(0.4));
        transform = glm::rotate(transform, glm::radians(float(glfwGetTime()) * 10), glm::vec3(1,1,0));
        glUniformMatrix4fv(u_transform_location, 1, GL_FALSE, glm::value_ptr(transform));

        // Spikes Torus WireFrame
        glBindVertexArray(spikestorusVAO.id);
        if(Globals.program == program1){
            glDrawElements(GL_LINE_STRIP, spikestorusVAO.element_array_count, GL_UNSIGNED_INT, 0);}
        
        else if(Globals.program == program2 | Globals.program == program3)
            glDrawElements(GL_TRIANGLES, spikestorusVAO.element_array_count, GL_UNSIGNED_INT, 0);
        
        else if(Globals.program == program4){
            // Spikes Torus Color
            glUniform3fv(u_color_location,1,glm::value_ptr(glm::vec3(0,1,0)));
            // Spikes Torus Shininess
            glUniform1i(u_shininess_location,256);
            glDrawElements(GL_TRIANGLES, spikestorusVAO.element_array_count, GL_UNSIGNED_INT, 0);
        }
        
        
        
        // Spikes Transformation
        transform = glm::mat4(1.0);
        transform = glm::translate(transform, glm::vec3(0.5, -0.5, 0));
        transform = glm::scale(transform,glm::vec3(0.4));
        transform = glm::rotate(transform, glm::radians(float(glfwGetTime()) * 10), glm::vec3(1,1,0));
        glUniformMatrix4fv(u_transform_location, 1, GL_FALSE, glm::value_ptr(transform));

        // Spikes WireFrame
        glBindVertexArray(spikesVAO.id);
        if(Globals.program == program1){
            glDrawElements(GL_LINE_STRIP, spikesVAO.element_array_count, GL_UNSIGNED_INT, 0);}
     
        else if(Globals.program == program2 | Globals.program == program3)
            glDrawElements(GL_TRIANGLES, spikesVAO.element_array_count, GL_UNSIGNED_INT, 0);
        
        else if(Globals.program == program4){
            // Spikes Color
            glUniform3fv(u_color_location,1,glm::value_ptr(glm::vec3(0,0,1)));
            // Spikes Shininess
            glUniform1i(u_shininess_location,512);
            glDrawElements(GL_TRIANGLES, spikesVAO.element_array_count, GL_UNSIGNED_INT, 0);
        }
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    

    
	glfwTerminate();
	return 0;
}
