#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdio>
#include <filesystem>

#include "mesh.h"
#include "image.h"
#include "rasterizer.h"

int main() {
    constexpr int W = 800, H = 600;

    std::filesystem::create_directories("saves");

    // Scene setup
    RenderContext ctx;
    ctx.model = glm::rotate(glm::mat4(1.f), glm::radians(30.f), glm::vec3(0,1,0)) *
                glm::rotate(glm::mat4(1.f), glm::radians(15.f), glm::vec3(1,0,0));

    ctx.view  = glm::lookAt(glm::vec3(1.5f, 1.8f, 3.2f),
                             glm::vec3(0.f,  0.f,  0.f),
                             glm::vec3(0.f,  1.f,  0.f));

    ctx.proj  = glm::perspective(glm::radians(55.f), float(W) / H, 0.1f, 100.f);

    ctx.light.lightPos   = {4.f,  5.f, 4.f};
    ctx.light.lightColor = {1.f,  1.f, 1.f};
    ctx.light.viewPos    = {1.5f, 1.8f, 3.2f};

    // Render first cube
    Mesh cube = makeCube();
    Image img(W, H);
    renderMesh(cube, ctx, img);

    // Render a second smaller cube offset to the right
    RenderContext ctx2 = ctx;
    ctx2.model = glm::translate(glm::mat4(1.f), glm::vec3(1.4f, 0.f, 0.f)) *
                 glm::scale(glm::mat4(1.f), glm::vec3(0.55f)) *
                 glm::rotate(glm::mat4(1.f), glm::radians(-40.f), glm::vec3(0,1,0)) *
                 glm::rotate(glm::mat4(1.f), glm::radians(20.f),  glm::vec3(1,0,0));
    ctx2.view = ctx.view;
    ctx2.proj = ctx.proj;
    renderMesh(cube, ctx2, img);

    // Render a third even smaller cube to the left
    RenderContext ctx3 = ctx;
    ctx3.model = glm::translate(glm::mat4(1.f), glm::vec3(-1.3f, -0.15f, 0.f)) *
                 glm::scale(glm::mat4(1.f), glm::vec3(0.4f)) *
                 glm::rotate(glm::mat4(1.f), glm::radians(60.f), glm::vec3(1,1,0));
    ctx3.view = ctx.view;
    ctx3.proj = ctx.proj;
    renderMesh(cube, ctx3, img);

    const char* outPath = "saves/render.png";
    if (img.save(outPath))
        std::printf("Saved: %s\n", outPath);
    else
        std::fprintf(stderr, "Failed to save %s\n", outPath);

    return 0;
}
