#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing fla

class CAssimpModel 
{ 
public: 
   bool LoadModelFromFile(char* sFilePath); 
   bool LoadModelFromFileBumpMap(char* sFilePath, char* sColorMap, char* sNormalMap); 

   static void FinalizeVBO(); 
   static void BindModelsVAO(); 

   void RenderModel(); 
   void RenderModelBumpMap(CShaderProgram* spProgram); 

   CAssimpModel(); 
private: 
   bool bLoaded; 
   static CVertexBufferObject vboModelData; 
   static UINT uiVAO; 
   static vector<CTexture> tTextures; 
   vector<int> iMeshStartIndices; 
   vector<int> iMeshSizes; 
   vector<int> iMaterialIndices; 
   int iNumMaterials; 
};

bool CAssimpModel::LoadModelFromFile(char* sFilePath) 
{ 
   if(vboModelData.GetBufferID() == 0) 
   { 
      vboModelData.CreateVBO(); 
      tTextures.reserve(50); 
   } 
   Assimp::Importer importer; 
   const aiScene* scene = importer.ReadFile( sFilePath,  
      aiProcess_CalcTangentSpace       |  
      aiProcess_Triangulate            | 
      aiProcess_JoinIdenticalVertices  | 
      aiProcess_SortByPType); 

   if(!scene) 
   { 
      MessageBox(appMain.hWnd, "Couldn't load model ", "Error Importing Asset", MB_ICONERROR); 
      return false; 
   } 

   const int iVertexTotalSize = sizeof(aiVector3D)*2+sizeof(aiVector2D); 

   int iTotalVertices = 0; 

   FOR(i, scene->mNumMeshes) 
   { 
      aiMesh* mesh = scene->mMeshes[i]; 
      int iMeshFaces = mesh->mNumFaces; 
      iMaterialIndices.push_back(mesh->mMaterialIndex); 
      int iSizeBefore = vboModelData.GetCurrentSize(); 
      iMeshStartIndices.push_back(iSizeBefore/iVertexTotalSize); 
      FOR(j, iMeshFaces) 
      { 
         const aiFace& face = mesh->mFaces[j]; 
         FOR(k, 3) 
         { 
            aiVector3D pos = mesh->mVertices[face.mIndices[k]]; 
            aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[k]]; 
            aiVector3D normal = mesh->HasNormals() ? mesh->mNormals[face.mIndices[k]] : aiVector3D(1.0f, 1.0f, 1.0f); 
            vboModelData.AddData(&pos, sizeof(aiVector3D)); 
            vboModelData.AddData(&uv, sizeof(aiVector2D)); 
            vboModelData.AddData(&normal, sizeof(aiVector3D)); 
         } 
      } 
      int iMeshVertices = mesh->mNumVertices; 
      iTotalVertices += iMeshVertices; 
      iMeshSizes.push_back((vboModelData.GetCurrentSize()-iSizeBefore)/iVertexTotalSize); 
   } 
   iNumMaterials = scene->mNumMaterials; 

   vector<int> materialRemap(iNumMaterials); 

   FOR(i, iNumMaterials) 
   { 
      const aiMaterial* material = scene->mMaterials[i]; 
      int a = 5; 
      int texIndex = 0; 
      aiString path;  // filename

      if(material->GetTexture(aiTextureType_DIFFUSE, texIndex, &path) == AI_SUCCESS) 
      { 
         string sDir = GetDirectoryPath(sFilePath); 
         string sTextureName = path.data; 
         string sFullPath = sDir+sTextureName; 
         int iTexFound = -1; 
         FOR(j, ESZ(tTextures))if(sFullPath == tTextures[j].GetPath()) 
         { 
            iTexFound = j; 
            break; 
         } 
         if(iTexFound != -1)materialRemap[i] = iTexFound; 
         else 
         { 
            CTexture tNew; 
            tNew.LoadTexture2D(sFullPath, true); 
            materialRemap[i] = ESZ(tTextures); 
            tTextures.push_back(tNew); 
         } 
      } 
   } 

   FOR(i, ESZ(iMeshSizes)) 
   { 
      int iOldIndex = iMaterialIndices[i]; 
      iMaterialIndices[i] = materialRemap[iOldIndex]; 
   } 

   return bLoaded = true; 
}

void CAssimpModel::FinalizeVBO() 
{ 
   glGenVertexArrays(1, &uiVAO); 
   glBindVertexArray(uiVAO); 
   vboModelData.BindVBO(); 
   vboModelData.UploadDataToGPU(GL_STATIC_DRAW); 
   // Vertex positions
   glEnableVertexAttribArray(0); 
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2*sizeof(aiVector3D)+sizeof(aiVector2D), 0); 
   // Texture coordinates
   glEnableVertexAttribArray(1); 
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(aiVector3D)+sizeof(aiVector2D), (void*)sizeof(aiVector3D)); 
   // Normal vectors
   glEnableVertexAttribArray(2); 
   glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2*sizeof(aiVector3D)+sizeof(aiVector2D), (void*)(sizeof(aiVector3D)+sizeof(aiVector2D))); 
} 


void CAssimpModel::RenderModel() 
{ 
   if(!bLoaded)return; 
   int iNumMeshes = ESZ(iMeshSizes); 
   FOR(i, iNumMeshes) 
   { 
      int iMatIndex = iMaterialIndices[i]; 
      tTextures[iMatIndex].BindTexture(); 
      glDrawArrays(GL_TRIANGLES, iMeshStartIndices[i], iMeshSizes[i]); 
   } 
}
