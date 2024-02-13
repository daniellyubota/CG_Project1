#pragma once

class Model
{
private:
	Assimp::Importer importer;
	const aiScene* scene = nullptr;
	aiNode* root_node = nullptr;

	struct Mesh
	{
		unsigned int VAO, VBO1, VBO2, VBO3, EBO;

		std::vector<glm::vec3> vert_positions;
		std::vector<glm::vec3> vert_normals;
		std::vector<glm::vec2> tex_coords;
		std::vector<unsigned int> vert_indices;
		unsigned int tex_handle;
	};

	struct Texture
	{
		unsigned int textureID;
		std::string image_name;
	};

public:
	unsigned int num_meshes;
	std::vector<Mesh> mesh_list;
	std::vector<Texture> texture_list;

	Model(const char* model_path)
	{



		scene = importer.ReadFile(model_path, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_FlipUVs);


		load_model();

	}

private:
	void load_model()
	{
		if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
			std::cout << "Assimp importer.ReadFile (Error) -- " << importer.GetErrorString() << "\n";
		else
		{
			num_meshes = scene->mNumMeshes;
			mesh_list.resize(num_meshes);

			aiMesh* mesh{};
			int indices_offset = 0; // Not being used yet... i.e. indices_offset += mesh->mNumVertices; is commented further down.


			for (unsigned int i = 0; i < num_meshes; ++i)
			{
				mesh = scene->mMeshes[i];			

				aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];		


				for (unsigned int tex_count = 0; tex_count < material->GetTextureCount(aiTextureType_DIFFUSE); ++tex_count)
				{
					aiString string;
					material->GetTexture(aiTextureType_DIFFUSE, tex_count, &string);	


					int already_loaded = is_image_loaded(string.C_Str());

					if (already_loaded == -1)
					{
						bool load_success = false;
						unsigned int texture_handle = load_texture_image(string.C_Str(), load_success);

						if (load_success)
						{
							Texture texture;
							texture.image_name = string.C_Str();
							texture.textureID = texture_handle;

							texture_list.push_back(texture);
							mesh_list[i].tex_handle = texture_handle;
						}
					}
					else
						mesh_list[i].tex_handle = already_loaded;
				}

				for (unsigned int i2 = 0; i2 < mesh->mNumVertices; ++i2)
				{
					glm::vec3 position{};
					position.x = mesh->mVertices[i2].x;
					position.y = mesh->mVertices[i2].y;
					position.z = mesh->mVertices[i2].z;
					mesh_list[i].vert_positions.push_back(position);

					if (mesh->HasNormals())
					{
						glm::vec3 normal{};
						normal.x = mesh->mNormals[i2].x;
						normal.y = mesh->mNormals[i2].y;
						normal.z = mesh->mNormals[i2].z;
						mesh_list[i].vert_normals.push_back(normal);
					}
					else
						mesh_list[i].vert_normals.push_back(glm::vec3(0.0f, 0.0f, 0.0f));

					if (mesh->HasTextureCoords(0))
					{
						glm::vec2 tex_coords{};
						tex_coords.x = mesh->mTextureCoords[0][i2].x;
						tex_coords.y = mesh->mTextureCoords[0][i2].y;
						mesh_list[i].tex_coords.push_back(tex_coords);
					}
					else
						mesh_list[i].tex_coords.push_back(glm::vec2(0.0f, 0.0f));
				}

				for (unsigned int i3 = 0; i3 < mesh->mNumFaces; ++i3)
					for (unsigned int i4 = 0; i4 < mesh->mFaces[i3].mNumIndices; ++i4)
						mesh_list[i].vert_indices.push_back(mesh->mFaces[i3].mIndices[i4] + indices_offset);


				set_buffer_data(i);
			}
		}
	}

	void load_model_cout_console()
	{

		if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
			std::cout << "Assimp importer.ReadFile (Error) -- " << importer.GetErrorString() << "\n";
		else
		{
			num_meshes = scene->mNumMeshes;
			mesh_list.resize(num_meshes);

			std::cout << "\n\n   Start of Assimp Loading Meshes & Analysis";
			std::cout << "\n   -----------------------------------------";

			root_node = scene->mRootNode;

			std::cout << "\n   node->mNumMeshes: " << root_node->mNumMeshes;
			std::cout << "\n   node->mName.C_Str(): " << root_node->mName.C_Str();
			std::cout << "\n\n   node->mNumChildren: " << root_node->mNumChildren;

			for (unsigned int i = 0; i < root_node->mNumChildren; ++i)
			{
				std::cout << "\n   node->mChildren[i]->mName.C_Str(): " << root_node->mChildren[i]->mName.C_Str();
				std::cout << "\n   node->mChildren[i]->mNumMeshes: " << root_node->mChildren[i]->mNumMeshes;
			}
			std::cout << "\n\n   scene->HasMaterials(): " << scene->HasMaterials();

			for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
				std::cout << "\n   scene->mMaterials[i]->GetName(): " << scene->mMaterials[i]->GetName().C_Str();

			std::cout << "\n\n   scene->HasTextures(): " << scene->HasTextures();

			aiMesh* mesh{};

			int total_num_indices = 0;
			int indices_offset = 0;

			std::cout << "\n   scene->mNumMeshes: " << num_meshes;
			std::cout << "\n   ********************\n";

			for (unsigned int i = 0; i < num_meshes; ++i)
			{
				mesh = scene->mMeshes[i];

				std::cout << "\n\n   mesh->mMaterialIndex: " << mesh->mMaterialIndex;
				std::cout << "\n   ----------------------- ";
				std::cout << "\n   mesh->mName.C_Str(): " << mesh->mName.C_Str();

				aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex]; 

				std::cout << "\n\n   material->GetTexture(aiTextureType_DIFFUSE, tex_count, &string): " << material->GetTextureCount(aiTextureType_DIFFUSE);
				std::cout << "\n   material->GetTexture(aiTextureType_SPECULAR, tex_count, &string): " << material->GetTextureCount(aiTextureType_SPECULAR);
				std::cout << "\n   material->GetTexture(aiTextureType_AMBIENT, tex_count, &string): " << material->GetTextureCount(aiTextureType_AMBIENT) << "\n\n";

				unsigned int tex_count = 0;
				for (; tex_count < material->GetTextureCount(aiTextureType_DIFFUSE); ++tex_count) 
				{
					aiString string;
					material->GetTexture(aiTextureType_DIFFUSE, tex_count, &string);
					std::cout << "   material->GetTexture(aiTextureType_DIFFUSE, tex_count, &string): " << string.C_Str() << "\n\n";

					int already_loaded = is_image_loaded(string.C_Str()); 
					std::cout << "   Loading Image\n";

					if (already_loaded == -1) 
					{
						bool load_complete = false;
						unsigned int texture_handle = load_texture_image(string.C_Str(), load_complete);

						if (load_complete)
						{
							Texture texture;
							texture.image_name = string.C_Str();
							texture.textureID = texture_handle;

							texture_list.push_back(texture);
							mesh_list[i].tex_handle = texture_handle;
						}
					}
					else
					{
						std::string edited = string.C_Str();
						std::size_t position = edited.find_last_of("\\");

						std::cout << "   Image file: " << edited.substr(position + 1) << " (is already loaded)";
						mesh_list[i].tex_handle = already_loaded;
					}
				}
				if (tex_count == 0)
					std::cout << "   material->GetTexture(aiTextureType_DIFFUSE, tex_count, &string): No image has been applied to this mesh\n\n";
				else
					std::cout << "\n";

				for (unsigned int slot = 0; slot < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++slot)
					std::cout << "   mesh->HasTextureCoords(" << slot << "): " << mesh->HasTextureCoords(slot) << "\n";

				std::cout << "\n   Mesh index: " << i << " (mesh->mNumVertices: " << mesh->mNumVertices << ")";
				std::cout << "\n   ------------------------------------- ";

				for (unsigned int i2 = 0; i2 < mesh->mNumVertices; ++i2)
				{
					glm::vec3 position{};
					position.x = mesh->mVertices[i2].x;
					position.y = mesh->mVertices[i2].y;
					position.z = mesh->mVertices[i2].z;
					mesh_list[i].vert_positions.push_back(position);

					std::cout << "\n   Count: " << i2;
					std::cout << "\n   mesh->mVertices[" << i2 << "].x: " << position.x;
					std::cout << "\n   mesh->mVertices[" << i2 << "].y: " << position.y;
					std::cout << "\n   mesh->mVertices[" << i2 << "].z: " << position.z;

					if (mesh->HasNormals())
					{
						glm::vec3 normal{};
						normal.x = mesh->mNormals[i2].x;
						normal.y = mesh->mNormals[i2].y;
						normal.z = mesh->mNormals[i2].z;
						mesh_list[i].vert_normals.push_back(normal);
						std::cout << "\n   mesh->mNormals[" << i2 << "] X: " << normal.x << " Y: " << normal.y << " Z: " << normal.z;
					}
					else
						mesh_list[i].vert_normals.push_back(glm::vec3(0.0f, 0.0f, 0.0f));

					if (mesh->HasTextureCoords(0))
					{
						glm::vec2 tex_coords{};
						tex_coords.x = mesh->mTextureCoords[0][i2].x;
						tex_coords.y = mesh->mTextureCoords[0][i2].y;
						mesh_list[i].tex_coords.push_back(tex_coords);
						std::cout << "\n   mesh->mTextureCoords[0][" << i2 << "] X: " << tex_coords.x << " Y: " << tex_coords.y;
					}
					else
						mesh_list[i].tex_coords.push_back(glm::vec2(0.0f, 0.0f));
				}
				std::cout << "\n\n   mesh->mNumFaces: " << mesh->mNumFaces << "\n";
				std::cout << "   ------------------ ";

				for (unsigned int i3 = 0; i3 < mesh->mNumFaces; ++i3)
				{
					std::cout << "\n";
					for (unsigned int i4 = 0; i4 < mesh->mFaces[i3].mNumIndices; ++i4)
					{
						std::cout << "   mesh->mFaces[" << i3 << "].mIndices[" << i4 << "]: " << mesh->mFaces[i3].mIndices[i4] << "\n";
						mesh_list[i].vert_indices.push_back(mesh->mFaces[i3].mIndices[i4] + indices_offset);
						++total_num_indices;
					}
				}
				std::cout << "\n   Total number of indices: " << total_num_indices;
				std::cout << "\n   **************************";
				total_num_indices = 0;

				std::cout << "\n   Indices offset (Total 'mesh->mNumVertices' so far): " << indices_offset;
				std::cout << "\n   *****************************************************\n\n";

				set_buffer_data(i);
			}
			if (texture_list.size() > 0)
				for (unsigned int i = 0; i < texture_list.size(); ++i)
				{
					std::cout << "   image_list[" << i << "].imageID: " << texture_list[i].textureID << "... image_list[" << i << "].image_name: " << texture_list[i].image_name << "\n";

					for (unsigned int i2 = 0; i2 < num_meshes; ++i2)
						if (texture_list[i].textureID == mesh_list[i2].tex_handle)
							std::cout << "   mesh_list[" << i2 << "].tex_handle: " << mesh_list[i2].tex_handle << "\n";
					std::cout << "\n";
				}
			else
				std::cout << "   ***** No images have been loaded\n";
		}
	}

	void set_buffer_data(unsigned int index)
	{
		glGenVertexArrays(1, &mesh_list[index].VAO);
		glGenBuffers(1, &mesh_list[index].VBO1);
		glGenBuffers(1, &mesh_list[index].VBO2);
		glGenBuffers(1, &mesh_list[index].VBO3);
		glGenBuffers(1, &mesh_list[index].EBO);

		glBindVertexArray(mesh_list[index].VAO);


		glBindBuffer(GL_ARRAY_BUFFER, mesh_list[index].VBO1);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh_list[index].vert_positions.size(), &mesh_list[index].vert_positions[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


		glBindBuffer(GL_ARRAY_BUFFER, mesh_list[index].VBO2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh_list[index].vert_normals.size(), &mesh_list[index].vert_normals[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


		glBindBuffer(GL_ARRAY_BUFFER, mesh_list[index].VBO3);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * mesh_list[index].tex_coords.size(), &mesh_list[index].tex_coords[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);


		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_list[index].EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh_list[index].vert_indices.size(), &mesh_list[index].vert_indices[0], GL_STATIC_DRAW);

		glBindVertexArray(0);
	}

	int is_image_loaded(std::string file_name)
	{
		for (unsigned int i = 0; i < texture_list.size(); ++i)
			if (file_name.compare(texture_list[i].image_name) == 0)
				return texture_list[i].textureID;
		return -1;
	}

	unsigned int load_texture_image(std::string file_name, bool& load_complete)
	{		

		std::size_t position = file_name.find_last_of("\\");
		file_name = "Images\\" + file_name.substr(position + 1);

		int width, height, num_components;
		unsigned char* image_data = stbi_load(file_name.c_str(), &width, &height, &num_components, 0);

		unsigned int textureID;
		glGenTextures(1, &textureID);

		if (image_data)
		{
			GLenum format{};

			if (num_components == 1)
				format = GL_RED;
			else if (num_components == 3)
				format = GL_RGB;
			else if (num_components == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image_data);
			glGenerateMipmap(GL_TEXTURE_2D);


			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); 
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);


			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // GL_NEAREST or GL_LINEAR.

			load_complete = true;
			stbi_image_free(image_data);
			std::cout << "   Image loaded OK: " << file_name << "\n";
		}
		else
		{
			load_complete = false;
			stbi_image_free(image_data);
			std::cout << "   Image failed to load: " << file_name << "\n";
		}
		return textureID;
	}
};