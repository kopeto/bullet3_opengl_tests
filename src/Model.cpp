#include "Model.hpp"

// constructor, expects a filepath to a 3D model.
Model::Model(string const &path, bool _fliptexture, bool gamma) : fliptexture{_fliptexture}, gammaCorrection(gamma)
{
    loadModel(path);
}

// draws the model, and thus all its meshes
void Model::Draw(Shader &shader) const
{
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);
}

void Model::Draw() const
{
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(*default_shader);
}

// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void Model::loadModel(string const &path)
{
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
        return;
    }
    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));

    this->name = scene->mName.C_Str();

    if (scene->HasLights())
    {
        for (unsigned int i = 0; i < scene->mNumLights; i++)
        {
            aiLight *light = scene->mLights[i];
            lights.push_back(processLight(light, scene));
        }
    }

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene);
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::processNode(aiNode *node, const aiScene *scene)
{
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene.
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];

        meshes.push_back(processMesh(mesh, scene));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    // data to fill
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // process materials
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    // return a mesh object created from the extracted mesh data
    return Mesh(vertices, indices, textures, mesh->mName.C_Str());
}

Light Model::processLight(aiLight *light, const aiScene *scene)
{
    std::string name;
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 colorAmbient;
    glm::vec3 colorDiffuse;
    glm::vec3 colorSpecular;

    name = light->mName.C_Str();
    position.x = light->mPosition.x;
    position.y = light->mPosition.y;
    position.z = light->mPosition.z;

    direction.x = light->mDirection.x;
    direction.y = light->mDirection.y;
    direction.z = light->mDirection.z;

    colorAmbient.x = light->mColorAmbient.r;
    colorAmbient.y = light->mColorAmbient.g;
    colorAmbient.z = light->mColorAmbient.b;

    colorDiffuse.x = light->mColorDiffuse.r;
    colorDiffuse.y = light->mColorDiffuse.g;
    colorDiffuse.z = light->mColorDiffuse.b;

    colorSpecular.x = light->mColorSpecular.r;
    colorSpecular.y = light->mColorSpecular.g;
    colorSpecular.z = light->mColorSpecular.b;

    return Light(name, position, direction, colorAmbient, colorDiffuse, colorSpecular);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
    vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if (!skip)
        { // if texture hasn't been loaded already, load it
            Texture texture(str.C_Str(), this->directory, fliptexture);
            texture.type = typeName;
            texture.path = str.C_Str();
            texture.name = mat->GetName().C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture); // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
        }
    }
    return textures;
}

unsigned int Model::GetRawPositions(float **positions)
{
    unsigned int n = 0;
    for (const auto &m : meshes)
    {
        n += m.vertices.size();
    }

    (*positions) = new float[n * 3];

    unsigned int i = 0;
    for (const auto &m : meshes)
    {
        for (const auto &v : m.vertices)
        {
            (*positions)[i++] = v.Position.x;
            (*positions)[i++] = v.Position.y;
            (*positions)[i++] = v.Position.z;
        }
    }

    return i;
}

void Model::setShader(Shader *shader)
{
    default_shader = shader;
}

void Model::info()
{
    // Model INFO:
    unsigned int nVert = 0;
    std::cout << "Model: " << name << "\n";
    std::cout << "Meshes: " << meshes.size() << "\n";
    for (const auto &mesh : meshes)
    {
        std::cout << "\t" << mesh.name << "\n";
        nVert += mesh.vertices.size();
    }
    std::cout << "Textures: " << textures_loaded.size() << "\n";
    for (const auto &texture : textures_loaded)
    {
        std::cout << "\t" << texture.name << " - " << texture.path << "\n";
    }
    std::cout << "Lights: " << lights.size() << "\n";
    for (const auto &l : lights)
    {
        std::cout << "\t" << l.mName << "\n";
    }
    std::cout << "Vertices: " << nVert << "\n";
}

SphereModel::SphereModel() // PROTOTYPE
{
    Init();
}

SphereModel::SphereModel(SphereModel &prototype, glm::vec3 pos, glm::vec3 velocity)
{
    default_shader = prototype.default_shader;
    meshes.push_back(Mesh{});
    meshes[0].vertices = prototype.meshes[0].vertices;
    meshes[0].textures = prototype.meshes[0].textures;
    meshes[0].indices = prototype.meshes[0].indices;
    meshes[0].setupMesh();
    sphereMesh = prototype.sphereMesh;
    sphereMesh->setupMesh();

    scale = glm::vec3(radius);

    // SPHERE SHAPE
    btCollisionShape *sphereShape = new btSphereShape(scale.x);
    btTransform initTransform(btQuaternion(0, 0, 0, 1), btVector3(pos.x, pos.y, pos.z));
    btDefaultMotionState *sphereMotionState = new btDefaultMotionState(initTransform);

    float density = 50.0f;
    btScalar mass = density * 4.0 / 3.0 * glm::pi<float>() * radius * radius;

    btVector3 sphereInertia(1.0, 1.0, 1.0);
    sphereShape->calculateLocalInertia(mass, sphereInertia);
    btRigidBody::btRigidBodyConstructionInfo sphereRigicBodyCI(mass, sphereMotionState, sphereShape, sphereInertia);
    body = new btRigidBody(sphereRigicBodyCI);

    body->setRestitution(1.0f);
    body->setFriction(2.0f);
    body->setLinearVelocity({velocity.x, velocity.y, velocity.z});
}

void SphereModel::Draw() const
{
    sphereMesh->Draw(*default_shader);
}

void SphereModel::Init()
{
    loadModel("models/sphere.obj");
    meshes[0].textures.emplace_back("textures/brick.jpeg", true);
    sphereMesh = &meshes[0];
    sphereMesh->setupMesh();
}