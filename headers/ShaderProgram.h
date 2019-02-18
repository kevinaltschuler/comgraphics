#ifndef _SHADERPROGRAM_H_
#define _SHADERPROGRAM_H_

#include <OpenGLFunctions.h>
#include "ShaderLocationsVault.h"
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

namespace util
{	

   /*
    *
    * This class encapsulates in a very basic way, a GLSL shader program
    * Go through this code to see how to initialize and use a shader without using any Qt classes.
    * This is the "barebones" way of working with GLSL shaders.
    *
    * This is NOT related to the ShaderProgram class in Qt. This class is created
    * for simplicity, instead of relying on a specific library
    */

class ShaderProgram
{
    class ShaderInfo
    {
        friend class ShaderProgram;
    private:
       // is it a vertex shader, a fragment shader, a geometry shader,
       // a tesselation shader or none of the above?
        int type;
        string  filename; //the file that stores this shader
        int shader; //the ID for this shader after it has been compiled


    public:
        ShaderInfo(int type,string filename,int shader)
        {
            this->type = type;
            this->filename = filename;
            this->shader = shader;
        }

        ShaderInfo()
        {
            this->type = 0;
            this->filename = "";
            this->shader = 0;
        }

        ~ShaderInfo(){}
    };

public:
    ShaderProgram()
    {
        program = -1;
        enabled = false;
    }

    ~ShaderProgram()
    {
    }

    /*
     * Return the program ID of this program
     * \return the program ID
     */
    int getProgram()
    {
        return program;
    }

    /*
     * Create a new shader program with the given
     * source code for a vertex shader and fragment
     * shader
     * \param to provide access to opengl functions
     * \param vertShaderFile the file for the source code for the vertex shader.
     *        This file must be placed within the resources of this project
     *        for portability purposes.
     * \param fragShaderFile the file for the source code for the fragment
     *        shader. This file must be placed within the resources of this
     *        project for portability purposes.
     * \throws runtime_error if any error is encountered
     */
    void createProgram(OpenGLFunctions& gl,string vertShaderFile,string fragShaderFile) throw(runtime_error)
    {

        releaseShaders(gl);

        shaders[0] = ShaderInfo(GL_VERTEX_SHADER,vertShaderFile,-1);
        shaders[1] = ShaderInfo(GL_FRAGMENT_SHADER,fragShaderFile,-1);

        program = createShaders(gl);
    }

    /*
     * Releases the resources (i.e. memory locations) for this shader. This must be called
     * only after the shader program is used, and will not be used again.
     */
    void releaseShaders(OpenGLFunctions& gl)
    {
        for (int i=0;i<2;i++)
        {
            if (shaders[i].shader!=0)
            {
                gl.glDeleteShader(shaders[i].shader);
                shaders[i].shader = 0;
            }
        }
        if (program>=0)
            gl.glDeleteProgram(program);
        program = 0;
    }

    /*
     * Make this program "current". All data henceforth will be sent to this program.
     */
    void enable(OpenGLFunctions& gl)
    {
        gl.glUseProgram(program);
        enabled = true;
    }

    /*
     * Disable this program.
     */
    void disable(OpenGLFunctions& gl)
    {
        gl.glUseProgram(0);
        enabled = false;
    }

    /*
     * Returns the table of all shader variables for this shader.
     *
     *
     * \return a ShaderLocationsVault object that represents the table of all shader variables
     */

    ShaderLocationsVault getAllShaderVariables(OpenGLFunctions& gl)
    {
        ShaderLocationsVault vault;
        int numVars;

        gl.glGetProgramiv(program, GL_ACTIVE_UNIFORMS,&numVars);
        for (int i=0;i<numVars;i++)
        {
            GLint size;
            GLsizei length;
            GLenum type;
            char nameVar[80];
            gl.glGetActiveUniform(program,i,80,&length,&size,&type,nameVar);
            string name = nameVar;


            vault.add(name,getUniformLocation(gl,nameVar));
        }

        gl.glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES,&numVars);
        for (int i=0;i<numVars;i++)
        {
            GLint size;
            GLsizei length;
            GLenum type;
            char nameVar[80];
            gl.glGetActiveAttrib(program,i,80,&length,&size,&type,nameVar);
            string name = nameVar;

            vault.add(name,getAttributeLocation(gl,name));
        }
        return vault;

    }

private:
    int getUniformLocation(OpenGLFunctions& gl,const string& name)
    {
        bool enabledStatus = enabled;

        if (!enabledStatus) //if not enabled, enable it
            enable(gl);

        int id = gl.glGetUniformLocation(program,name.c_str());

        if (!enabledStatus) //if it was not enabled, disable it again
            disable(gl);

        return id;
    }

    int getAttributeLocation(OpenGLFunctions& gl,const string& name)
    {
        bool enabledStatus = enabled;

        if (!enabledStatus) //if not enabled, enable it
            enable(gl);

        int id = gl.glGetAttribLocation(program,name.c_str());

        if (!enabledStatus) //if it was not enabled, disable it again
            disable(gl);

        return id;
    }


    int createShaders(OpenGLFunctions& gl) throw(runtime_error)
    {
        ifstream file;
        GLint linked,compiled;

        program = gl.glCreateProgram();


        for (int i=0;i<2;i++)
        {
            file.open(shaders[i].filename.c_str());


            if (!file.is_open())
            {
                stringstream str;
                str << "Shader " << shaders[i].filename << "not found or could not be opened!" << endl;
                throw runtime_error(str.str());
            }

            string source,line;


            getline(file,line);
            while (!file.eof())
            {
                source = source + "\n" + line;
                getline(file,line);
            }
            file.close();


            const char *codev = source.c_str();


            shaders[i].shader = gl.glCreateShader(shaders[i].type);
            gl.glShaderSource(shaders[i].shader,1,&codev,NULL);
            gl.glCompileShader(shaders[i].shader);
            gl.glGetShaderiv(shaders[i].shader,GL_COMPILE_STATUS,&compiled);

            if (!compiled)
            {
                string error_message = printShaderInfoLog(gl,shaders[i].shader);
                releaseShaders(gl);
                throw runtime_error(error_message);
            }
            gl.glAttachShader(program, shaders[i].shader);
        }

        gl.glLinkProgram(program);
        gl.glGetProgramiv(program,GL_LINK_STATUS,&linked);

        if (!linked)
        {
            string error_message = printShaderInfoLog(gl,program);
            releaseShaders(gl);
            throw runtime_error(error_message);
        }

        return program;
    }

    string printShaderInfoLog(OpenGLFunctions& gl,GLuint shader)
    {
        int infologLen = 0;
        int charsWritten = 0;
        GLubyte *infoLog;
        stringstream str;

        gl.glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&infologLen);
        if (infologLen>0)
        {
            infoLog = new GLubyte[infologLen];
            if (infoLog != NULL)
            {
                gl.glGetShaderInfoLog(shader,infologLen,&charsWritten,(char *)infoLog);
                for (int i=0;i<charsWritten;i++)
                    str << infoLog[i];
                str << endl;
                delete []infoLog;
            }

        }
        return str.str();
    }

private:
    int program;
    ShaderInfo shaders[2];
    bool enabled;
    GLenum a;




};
}
#endif

