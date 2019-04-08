#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <vector>

namespace PostEffect {
typedef unsigned int uint32;
typedef int int32;

//Istead of ply quad
namespace fullscreen_quad {
// p.x p.y uv.x uv.y
const float quadVertices[] = {
    -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f};
} 

struct Texture2D {
  Texture2D() : id(UINT_MAX) {}
  Texture2D(uint32 in_width, int32 in_height, int32 in_pixelformat)
      : width(in_width), height(in_height), pixelformat(in_pixelformat) {
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexStorage2D(GL_TEXTURE_2D, 1, in_pixelformat, width, height);
  }
  Texture2D(uint32 in_width, int32 in_height, int32 in_pixelformat,
            int32 in_data_type)
      : width(in_width), height(in_height), pixelformat(in_pixelformat),
        data_type(in_data_type) {
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, pixelformat, width, height, 0, pixelformat,
                 data_type, NULL);
  }
  ~Texture2D() {
    // If it wasn't initialized there is nothing to delete
    if (id == UINT_MAX)
      return;
    glDeleteTextures(1, &id);
  }

  uint32 id;
  int32 width, height, pixelformat, data_type;
};

struct TextureRenderTarget {
  struct State {
    State(TextureRenderTarget &render_target) {
      glBindFramebuffer(GL_FRAMEBUFFER, render_target.fbo_id);
    }
    ~State() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
  };
  TextureRenderTarget(int32 width, int32 height, float scale_factor = 1.f)
      : texture((int32)(width * scale_factor), (int32)(height * scale_factor),
                GL_RGBA32F),
        screen_width(width), screen_height(height) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // framebuffer configuration
    // -------------------------
    glGenFramebuffers(1, &fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           texture.id, 0);
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width,
                          height); // use a single renderbuffer object for
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, rbo);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      printf("ERROR::TextureRenderTarget:: Framebuffer is not complete!\n");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  ~TextureRenderTarget() { glDeleteFramebuffers(1, &fbo_id); }

  int32 screen_width, screen_height;

  uint32 fbo_id;
  Texture2D texture;
  uint32 rbo;
};


struct RenderTarget {
  struct State {
    State(RenderTarget &render_target) {
      previous_dimensions[0] = render_target.screen_width;
      previous_dimensions[1] = render_target.screen_height;

      glBindFramebuffer(GL_FRAMEBUFFER, render_target.fbo_id);
      glViewport(0, 0, render_target.color.width, render_target.color.height);
    }
    State(TextureRenderTarget &render_target) {
      previous_dimensions[0] = render_target.screen_width;
      previous_dimensions[1] = render_target.screen_height;

      glBindFramebuffer(GL_FRAMEBUFFER, render_target.fbo_id);
      glViewport(0, 0, render_target.texture.width,
                 render_target.texture.height);
    }
    ~State() {
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glViewport(0, 0, previous_dimensions[0], previous_dimensions[1]);
    }
    int32 previous_dimensions[2] = {0, 0};
  };

  RenderTarget(int32 width, int32 height, float scale_factor = 1.f)
      : color((int32)(width * scale_factor), (int32)(height * scale_factor),
              GL_RGBA32F),
        normal((int32)(width * scale_factor), (int32)(height * scale_factor),
               GL_RGBA32F),
        world_position((int32)(width * scale_factor),
                       (int32)(height * scale_factor), GL_RGBA32F),
        depth((int32)(width * scale_factor), (int32)(height * scale_factor),
              GL_DEPTH24_STENCIL8),
        screen_height(height), screen_width(width) {
    set_texture_filter_parameter(color);
    set_texture_filter_parameter(normal);
    set_texture_filter_parameter(world_position);

    // framebuffer configuration
    // -------------------------
    glGenFramebuffers(1, &fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
    // create a color attachment texture
    // Color goes to #0
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color.id, 0);
    // Normal goes to #1
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, normal.id, 0);
    // Vertex world position #2
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
                         world_position.id, 0);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depth.id,
                         0);
    const GLenum draw_bufers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                                  GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, draw_bufers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void set_texture_filter_parameter(Texture2D &tex) {
    glBindTexture(GL_TEXTURE_2D, tex.id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }

  ~RenderTarget() { glDeleteFramebuffers(1, &fbo_id); }

  Texture2D color;
  Texture2D normal;
  Texture2D world_position;
  Texture2D depth;

  int32 screen_width, screen_height;

  uint32 fbo_id;
  uint32 rbo;
};

struct ScreenGeo {
  uint32 vao, vbo;
  ScreenGeo() {
    //initialization stuff here
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fullscreen_quad::quadVertices),
                 &fullscreen_quad::quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *)(2 * sizeof(float)));
    is_initialized = true;
  }
  ~ScreenGeo() {
    if (is_initialized) {
      glDeleteVertexArrays(1, &vao);
      glDeleteBuffers(1, &vbo);
      is_initialized = false;
    }
  }
  bool is_initialized;
};

struct Shader {
  uint32 ID;
  Shader(const char *vertex_source, const char *fragment_source) {
    load(vertex_source, fragment_source);
  }
  Shader(const char *vertexPath, const char *fragmentPath,
         bool load_from_files) {

    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
      // open files
      vShaderFile.open(vertexPath);
      fShaderFile.open(fragmentPath);
      std::stringstream vShaderStream, fShaderStream;
      // read file's buffer contents into streams
      vShaderStream << vShaderFile.rdbuf();
      fShaderStream << fShaderFile.rdbuf();
      // close file handlers
      vShaderFile.close();
      fShaderFile.close();
      // convert stream into string
      vertexCode = vShaderStream.str();
      fragmentCode = fShaderStream.str();
      load(vertexCode, fragmentCode);
    } catch (std::ifstream::failure e) {
      std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
  }
  void load(const std::string &vertexCode, const std::string &fragmentCode) {
    // 2. compile shaders
    uint32 vertex, fragment;
    const char *vertex_source = vertexCode.c_str();
    const char *fragment_source = fragmentCode.c_str();
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_source, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_source, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer
    // necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);
  }
  void use() const { glUseProgram(ID); }
  void set_texture(const std::string &name, int32 texture_unit,
                   Texture2D &texture) const {
    glActiveTexture(GL_TEXTURE0 + texture_unit);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    glUniform1i(glGetUniformLocation(ID, name.c_str()), texture_unit);
  }
  void set_bool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
  }
  void set_int(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
  }
  void set_float(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
  }
  void set_vec2(const std::string &name, const glm::vec2 &value) const {
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
  }
  void set_vec2(const std::string &name, float x, float y) const {
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
  }
  void set_vec3(const std::string &name, const glm::vec3 &value) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
  }
  void set_vec3(const std::string &name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
  }
  void set_vec4(const std::string &name, const glm::vec4 &value) const {
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
  }
  void set_vec4(const std::string &name, float x, float y, float z, float w) {
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
  }
  void set_mat2(const std::string &name, const glm::mat2 &mat) const {
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                       &mat[0][0]);
  }
  void set_mat3(const std::string &name, const glm::mat3 &mat) const {
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                       &mat[0][0]);
  }
  void set_mat4(const std::string &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                       &mat[0][0]);
  }

private:
  void checkCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
      glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
      if (!success) {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cout
            << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
            << infoLog
            << "\n -- --------------------------------------------------- -- "
            << std::endl;
      }
    } else {
      glGetProgramiv(shader, GL_LINK_STATUS, &success);
      if (!success) {
        glGetProgramInfoLog(shader, 1024, NULL, infoLog);
        std::cout
            << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
            << infoLog
            << "\n -- --------------------------------------------------- -- "
            << std::endl;
      }
    }
  }
};

struct ScreenQuad {
  ScreenQuad(int32 in_width, int32 in_height)
      : width(in_width), height(in_height),
#pragma region screen shader
        screen_shader(R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;
out vec2 TexCoords;
void main() {
  TexCoords = aTexCoords;
  gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
})",
                      R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D screenTexture;
void main() {
  vec3 col = texture(screenTexture, TexCoords).rgb;
  FragColor = vec4(col, 1.0);
})"),
#pragma endregion
        screen_geo() {
  }

  void draw_with_texture(Texture2D &source_texture) {
    draw(screen_shader, source_texture);
  }

  void draw(Shader &shader) {
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(screen_geo.vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }

  void draw_to_render_target(TextureRenderTarget &render_target,
                             Shader &shader) {
    RenderTarget::State state(render_target);
    draw(shader);
  }

  void blit(Texture2D &source_texture,
            TextureRenderTarget &destination_texture) {
    RenderTarget::State state(destination_texture);
    draw(screen_shader, source_texture);
  }

  Shader screen_shader;
  ScreenGeo screen_geo;

  int32 width, height;

  ~ScreenQuad() {}

private:
  void draw(Shader &shader, Texture2D &texture) {
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    shader.use();
    glBindVertexArray(screen_geo.vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.id);

    glDrawArrays(GL_TRIANGLES, 0, 6);
  }
};

struct PostEffect {
  PostEffect(ScreenQuad &in_screen_quad) : screen_quad(in_screen_quad) {}

  ScreenQuad &screen_quad;
};

struct BlurEffect : public PostEffect {
  BlurEffect(ScreenQuad &in_screen_quad)
      : PostEffect(in_screen_quad),
        blur_shader("assets/shaders/blur.vert", "assets/shaders/blur.frag",
                    true) {}
  void execute(TextureRenderTarget &from, TextureRenderTarget &to,
               int blur_iterations) {
    TextureRenderTarget *a = &from;
    TextureRenderTarget *b = &to;
    TextureRenderTarget *tmp;
    for (int32 x = 0; x < blur_iterations; x++) {
      {
        RenderTarget::State state(*b);
        blur_shader.use();
        blur_shader.set_bool("horizontal", x % 2);
        blur_shader.set_texture("image", 0, (*a).texture);
        screen_quad.draw(blur_shader);
      }
      tmp = a;
      a = b;
      b = tmp;
    }
  }
  Shader blur_shader;
};

// uses blur effect above
struct BloomEffect : public PostEffect {
  BloomEffect(ScreenQuad &in_screen_quad, float downscale_factor = 0.125f)
      : PostEffect(in_screen_quad), blur_effect(in_screen_quad), blurred_a(screen_quad.width, screen_quad.height, downscale_factor),
      blurred_b(screen_quad.width, screen_quad.height, downscale_factor),
#pragma region blur shader
        bloom_shader("assets/shaders/blur.vert", "assets/shaders/bloom.frag",
                     true)
#pragma endregion
  {
  }

  void execute(TextureRenderTarget &output, Texture2D &input, float strength = 1.f,
               int blur_iterations = 10, 
               float threshold = .93f) {
    // downscale scene texture and write it to A
    screen_quad.blit(input, blurred_a);
    blur_effect.execute(blurred_a, blurred_b, blur_iterations);
    // Composite with the scene texture.
    composite(output, input, blurred_a.texture, strength, threshold);
  }

  void composite(TextureRenderTarget &rt, Texture2D &scene_texture,
                 Texture2D &blurred_texture, float strength, float threshold) {
    RenderTarget::State state(rt);
    bloom_shader.use();
    bloom_shader.set_texture("SceneTexture", 1, scene_texture);
    bloom_shader.set_float("threshold", threshold);
    bloom_shader.set_float("Strength", strength);
    bloom_shader.set_texture("image", 0, blurred_texture);
    screen_quad.draw(bloom_shader);
  }

  TextureRenderTarget blurred_a;
  TextureRenderTarget blurred_b;

  Shader bloom_shader;
  BlurEffect blur_effect;
};


struct ChromAberrationEffect : public PostEffect {
	ChromAberrationEffect(ScreenQuad &in_screen_quad)
		: PostEffect(in_screen_quad),
		ChromAberration_shader("assets/shaders/blur.vert", "assets/shaders/ChromAberration.frag",
			true) {}
	void execute(TextureRenderTarget &output, Texture2D &color) {
		RenderTarget::State state(output);
		ChromAberration_shader.use();
		ChromAberration_shader.set_texture("SceneTexture", 0, color);
		screen_quad.draw(ChromAberration_shader);
	}
	Shader ChromAberration_shader;
};

} // namespace postEffect
