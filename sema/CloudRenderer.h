#ifndef CLOUD_RENDERER_H
#define CLOUD_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <algorithm> 
#include <cstdlib>   
#include "Shader.h"


struct CloudParticle {
    glm::vec3 position;
    float scale;
    float alpha;
    float distToCam; 
};

class CloudRenderer {
private:
    unsigned int VAO, VBO_Quad, VBO_Instance;
    unsigned int textureID;
    Shader* shader;
    std::vector<CloudParticle> particles;

public:
    
    CloudRenderer(unsigned int texID, Shader* s) : textureID(texID), shader(s) {
        setupMesh();
        initParticles();
    }

    
    void initParticles() {
        particles.clear();
        for (int i = 0; i < 500; i++) {
            CloudParticle p;
            
            float x = static_cast<float>(rand() % 100 - 50);
            float y = static_cast<float>(rand() % 15 + 10); 
            float z = static_cast<float>(rand() % 100 - 50);

            p.position = glm::vec3(x, y, z);
            p.scale = static_cast<float>(rand() % 10 + 10); 
            p.alpha = 0.8f;
            particles.push_back(p);
        }
    }

    
    void setupMesh() {
        
        float quadVertices[] = {
            
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
             0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
             0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
             0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
            -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO_Quad);
        glGenBuffers(1, &VBO_Instance);

        glBindVertexArray(VAO);

        
        glBindBuffer(GL_ARRAY_BUFFER, VBO_Quad);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, VBO_Instance);
        glBufferData(GL_ARRAY_BUFFER, 1000 * 5 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glVertexAttribDivisor(2, 1); 
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glVertexAttribDivisor(3, 1);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(4 * sizeof(float)));
        glVertexAttribDivisor(4, 1);
        glBindVertexArray(0);
    }

    void sortParticles(glm::vec3 camPos) {
        for (auto& p : particles) {
            p.distToCam = glm::length(camPos - p.position);
        }
        std::sort(particles.begin(), particles.end(),
            [](const CloudParticle& a, const CloudParticle& b) {
                return a.distToCam > b.distToCam; 
            });
    }


    void draw(glm::mat4 view, glm::mat4 proj, unsigned int depthTex, glm::vec3 camPos, float screenWidth, float screenHeight) {
        sortParticles(camPos);
        std::vector<float> instanceData;
        instanceData.reserve(particles.size() * 5);
        for (const auto& p : particles) {
            instanceData.push_back(p.position.x);
            instanceData.push_back(p.position.y);
            instanceData.push_back(p.position.z);
            instanceData.push_back(p.scale);
            instanceData.push_back(p.alpha);
        }

        glBindBuffer(GL_ARRAY_BUFFER, VBO_Instance);
        glBufferData(GL_ARRAY_BUFFER, instanceData.size() * sizeof(float), instanceData.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        shader->use();
        shader->setMat4("view", view);
        shader->setMat4("projection", proj);
        shader->setVec2("u_ScreenSize", screenWidth, screenHeight);
        shader->setFloat("u_Softness", 45.0f); 
        shader->setInt("u_CloudTexture", 0); 
        shader->setInt("u_SceneDepth", 1);   

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthTex);
        glBindVertexArray(VAO);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE); 

        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, particles.size());

 
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glBindVertexArray(0);
    }
};

#endif