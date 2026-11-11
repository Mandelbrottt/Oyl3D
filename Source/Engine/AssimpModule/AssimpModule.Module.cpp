#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <Core/Containers/ArraySet.h>
#include <Core/Containers/Optional.h>
#include <Core/Modules/ModuleManager.h>

#include <Rendering/Rendering.h>

namespace Oyl
{
	class AssimpModule final
		: public IModelLoaderFactory,
		  public IModuleInterface
	{
	public:
		void
		OnStartModule() override
		{
			PopulateSupportedExtensions();

			auto* renderingModule = ModuleManager::GetModulePointer<IRenderingModule>("Rendering");
			if (renderingModule)
				renderingModule->RegisterModelLoaderFactory(*this);
		}

		void
		OnStopModule() override
		{
			auto* renderingModule = ModuleManager::GetModulePointer<IRenderingModule>("Rendering");
			if (renderingModule)
				renderingModule->UnregisterModelLoaderFactory(*this);
		}

		bool ShouldAutoload() override
		{
			return true;
		}

		bool
		SupportsHotReload() override
		{
			return false;
		}

		bool
		SupportsExtension(StringView a_extension) override
		{
			return m_supportedExtensions.Contains(a_extension);
		}

		Optional<Model>
		LoadModelFromFile(StringView a_fileName) override
		{
			Assimp::Importer importer;

			const aiScene* pScene = importer.ReadFile(a_fileName.Data(),
			                                          aiProcess_Triangulate
			                                          | aiProcess_ConvertToLeftHanded);

			if (pScene == nullptr)
				return {};

			Array<Mesh> meshes;
			ProcessNode(pScene->mRootNode, pScene, &meshes);
			return Model(meshes);
		}

	private:
		void
		PopulateSupportedExtensions()
		{
			m_supportedExtensions.Add(".obj");
			m_supportedExtensions.Add(".fbx");
		}

		static
		void
		ProcessNode(const aiNode* a_node, const aiScene* a_scene, Array<Mesh>* a_outMeshes)
		{
			OYL_ASSERT(a_node && a_scene && a_outMeshes);

			for (uint i = 0; i < a_node->mNumMeshes; i++)
			{
				uint meshIndex = a_node->mMeshes[i];
				aiMesh* mesh = a_scene->mMeshes[meshIndex];
				a_outMeshes->Add(ProcessMesh(mesh, a_scene));
			}

			for (uint i = 0; i < a_node->mNumChildren; i++)
			{
				aiNode* child = a_node->mChildren[i];
				ProcessNode(child, a_scene, a_outMeshes);
			}
		}

		static
		Mesh
		ProcessMesh(const aiMesh* a_mesh, const aiScene* a_scene)
		{
			Array<Vector3> vertexPositions;
			Array<Vector2> vertexUvs;
			Array<Vector3> vertexNormals;
			Array<uint32> vertexIndices;

			for (uint32 i = 0; i < a_mesh->mNumVertices; i++)
			{
				const auto& inPosition = a_mesh->mVertices[i];
				Vector3 position = { inPosition.x, inPosition.y, inPosition.z };
				vertexPositions.Add(position);

				if (a_mesh->HasTextureCoords(0))
				{
					const auto* inUv = a_mesh->mTextureCoords[0];
					Vector2 uv = inUv
						             ? Vector2f(inUv[i].x, inUv[i].y)
						             : Vector2f::Zero;
					vertexUvs.Add(uv);
				}

				if (a_mesh->HasNormals())
				{
					const auto& inNormal = a_mesh->mNormals[i];
					Vector3 normal = { inNormal.x, inNormal.y, inNormal.z };
					vertexNormals.Add(normal);
				}
			}

			for (uint32 i = 0; i < a_mesh->mNumFaces; i++)
			{
				aiFace inFace = a_mesh->mFaces[i];

				for (uint32 j = 0; j < inFace.mNumIndices; j++)
					vertexIndices.Add(inFace.mIndices[j]);
			}

			// TODO: Textures
			OYL_UNUSED(a_scene);

			return Mesh(
				vertexPositions,
				vertexUvs,
				vertexNormals,
				vertexIndices
			);
		}

		ArraySet<String> m_supportedExtensions;
	};

	OYL_MODULE_DECLARE(AssimpModule, AssimpModule)
}
