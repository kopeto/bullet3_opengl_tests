#ifndef MODEL_H
#define MODEL_H


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb/stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include "Shader.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"
#include "Light.hpp"

#include <btBulletDynamicsCommon.h>


class Model 
{
public:
    // model data 
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh>    meshes;

    std::string name;
    string directory;

    bool fliptexture = true;
    bool gammaCorrection;

    // Default constructor;
    Model() = default;

    // constructor, expects a filepath to a 3D model.
    Model(string const &path, bool fliptexture, bool gamma = false);

    // draws the model, and thus all its meshes
    void Draw(Shader &shader) const;
    unsigned int GetRawPositions(float **positions) const;
    std::vector<float> GetRawPositions() const;

    void info();
    void Init(std::string , std::string);


protected:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const &path);

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene);

    Light   processLight(aiLight *light, const aiScene *scene);
    Mesh    processMesh(aiMesh *mesh, const aiScene *scene);

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);

};


struct SphereModel : public Model
{
    SphereModel(std::string name = "");

    float radius = 1.0f;

    void Draw() const;

};

struct FlatTerrain : public Model
{
    FlatTerrain(std::string name  = "");

    float w,h;

    void Draw() const;

};

struct CubeModel : public Model
{
    CubeModel(std::string name = "");

    glm::vec3 dims;

    void Draw() const;

};

struct DonutModel : public Model
{
    DonutModel(std::string name = "");

    float radius = 1.0f;

    void Draw() const;
};

struct HalfDonutModel : public Model
{
    HalfDonutModel(std::string name = "");

    float radius = 1.0f;

    void Draw() const;
};



#endif
