#include "OBJMgr.h"
#include "SceneMgr.h"
#include "Texture.h"
#include "Utility.h"
#include "Material.h"
#include "MathUtils.h"

#include <cmath>
#include <algorithm>
#include <iterator>

#define MOON_DEBUG_MODE

namespace MOON {

	int GetLineCount(const char* filePath) {
		std::ifstream myfile(filePath);

		// new lines will be skipped unless we stop it from happening:    
		myfile.unsetf(std::ios_base::skipws);

		// count the newlines with an algorithm specialized for counting:
		unsigned line_count = std::count(
			std::istream_iterator<char>(myfile),
			std::istream_iterator<char>(), '\n'
		);

		return line_count;
	}

	// Split a String into a string array at a given token
	void split(const std::string &in, std::vector<std::string> &out, std::string token) {
		out.clear();
		std::string temp;
		for (int i = 0; i < int(in.size()); i++) {
			std::string test = in.substr(i, token.size());
			if (test == token) {
				if (!temp.empty()) {
					out.push_back(temp);
					temp.clear();
					i += (int)token.size() - 1;
				} else {
					out.push_back("");
				}
			} else if (i + token.size() >= in.size()) {
				temp += in.substr(i, token.size());
				out.push_back(temp);
				break;
			} else {
				temp += in[i];
			}
		}
	}

	// Get tail of string after first token and possibly following spaces
	std::string tail(const std::string &in) {
		size_t token_start = in.find_first_not_of(" \t");
		size_t space_start = in.find_first_of(" \t", token_start);
		size_t tail_start = in.find_first_not_of(" \t", space_start);
		size_t tail_end = in.find_last_not_of(" \t");
		if (tail_start != std::string::npos && tail_end != std::string::npos) {
			return in.substr(tail_start, tail_end - tail_start + 1);
		} else if (tail_start != std::string::npos) {
			return in.substr(tail_start);
		}
		return "";
	}

	// Get first token of string
	std::string firstToken(const std::string &in) {
		if (!in.empty()) {
			size_t token_start = in.find_first_not_of(" \t");
			size_t token_end = in.find_first_of(" \t", token_start);
			if (token_start != std::string::npos && token_end != std::string::npos) {
				return in.substr(token_start, token_end - token_start);
			} else if (token_start != std::string::npos) {
				return in.substr(token_start);
			}
		}
		return "";
	}

	void OBJLoader::GetInfo(std::string& info, float& progress) {
		info = OBJLoader::info;
		progress = OBJLoader::progress;
	}

	void OBJLoader::LoadFile(Model* container) {
		LoadFile(container->path, container->meshList);
	}

	bool OBJLoader::LoadFile(const std::string &Path, std::vector<Mesh*> &LoadedMeshes) {
		// If the file is not an .obj file return false
		if (Path.substr(Path.size() - 4, 4) != ".obj") return false;
		std::ifstream file(Path); if (!file.is_open()) return false;

		unsigned int lineCount = GetLineCount(Path.c_str());
		unsigned int currLineCnt = 0;

		// Re-init
		OBJLoader::progress = 0;
		OBJLoader::info = "Loading... ...";
		LoadedMeshes.clear();
		LoadedVertices.clear();
		LoadedIndices.clear();

		std::vector<Vector3> Positions;
		std::vector<Vector2> TCoords;
		std::vector<Vector3> Normals;
		std::vector<Vertex> Vertices;
		std::vector<unsigned int> Indices;
		std::vector<std::string> MeshMatNames;

		bool listening = false;
		std::string meshname;
		std::string curline;

		Mesh* tempMesh;

#ifdef MOON_DEBUG_MODE
		std::cout << std::endl;
		std::cout << "Load model from: " << Path << std::endl;
#endif

		while (std::getline(file, curline)) {
			OBJLoader::progress = (float)(currLineCnt++) / lineCount;
			//std::cout << OBJLoader::progress << std::endl;
			// Generate a Mesh Object or Prepare for an object to be created
			if (firstToken(curline) == "o" || firstToken(curline) == "g" || curline[0] == 'g') {
				if (!listening) {
					listening = true;
					if (firstToken(curline) == "o" || firstToken(curline) == "g")
						meshname = tail(curline);
					else meshname = "unnamed";
				} else {
					// Generate the mesh to put into the array
					if (!Indices.empty() && !Vertices.empty()) {
						// Create Mesh
						tempMesh = new Mesh(meshname, Vertices, Indices);

						// Insert Mesh
						LoadedMeshes.push_back(tempMesh);

						// Cleanup
						Vertices.clear();
						Indices.clear();
						meshname.clear();

						meshname = tail(curline);
					} else {
						if (firstToken(curline) == "o" || firstToken(curline) == "g")
							meshname = tail(curline);
						else meshname = "unnamed";
					}
				}
			}
			// Generate a Vertex Position
			OBJLoader::info = "Load Vertex Position...";
			if (firstToken(curline) == "v") {
				std::vector<std::string> spos;
				Vector3 vpos;
				split(tail(curline), spos, " ");

				vpos.x = std::stof(spos[0]);
				vpos.y = std::stof(spos[1]);
				vpos.z = std::stof(spos[2]);

				Positions.push_back(vpos);
			}
			// Generate a Vertex Texture Coordinate
			OBJLoader::info = "Load Texture Coordinate...";
			if (firstToken(curline) == "vt") {
				std::vector<std::string> stex;
				Vector2 vtex;
				split(tail(curline), stex, " ");

				vtex.x = std::stof(stex[0]);
				vtex.y = std::stof(stex[1]);

				TCoords.push_back(vtex);
			}
			// Generate a Vertex Normal;
			OBJLoader::info = "Load Vertex Normal...";
			if (firstToken(curline) == "vn") {
				std::vector<std::string> snor;
				Vector3 vnor;
				split(tail(curline), snor, " ");

				vnor.x = std::stof(snor[0]);
				vnor.y = std::stof(snor[1]);
				vnor.z = std::stof(snor[2]);

				Normals.push_back(vnor);
			}
			// Generate a Face (vertices & triangles)
			OBJLoader::info = "Load Face...";
			if (firstToken(curline) == "f") {
				// Generate the vertices
				std::vector<Vertex> vVerts;
				GenVerticesFromRawOBJ(vVerts, Positions, TCoords, Normals, curline);

				// Add Vertices
				for (int i = 0; i < int(vVerts.size()); i++) {
					Vertices.push_back(vVerts[i]);
					LoadedVertices.push_back(vVerts[i]);
				}

				std::vector<unsigned int> iIndices;
				VertexTriangluation(iIndices, vVerts);

				// Add Indices
				for (int i = 0; i < int(iIndices.size()); i++) {
					unsigned int indnum = (unsigned int)((Vertices.size()) - vVerts.size()) + iIndices[i];
					Indices.push_back(indnum);

					indnum = (unsigned int)((LoadedVertices.size()) - vVerts.size()) + iIndices[i];
					LoadedIndices.push_back(indnum);
				}
			}
			// Get Mesh Material Name
			if (firstToken(curline) == "usemtl") {
				MeshMatNames.push_back(tail(curline));

				// Create new Mesh, if Material changes within a group
				if (!Indices.empty() && !Vertices.empty()) {
					// Create Mesh
					tempMesh = new Mesh(meshname, Vertices, Indices);
					int i = 2;
					while (1) {
						tempMesh->name = meshname + "_" + std::to_string(i);
						for (auto &m : LoadedMeshes)
							if (m->name == tempMesh->name)
								continue;
						break;
					}

					// Insert Mesh
					LoadedMeshes.push_back(tempMesh);

					// Cleanup
					Vertices.clear();
					Indices.clear();
				}

			}
			// Load Materials
			OBJLoader::info = "Load Materials...";
			if (firstToken(curline) == "mtllib") {
				// Generate LoadedMaterial
				// Generate a path to the material file
				std::vector<std::string> temp;
				split(Path, temp, "\\");

				std::string pathtomat = "";
				if (temp.size() != 1) {
					for (int i = 0; i < temp.size() - 1; i++)
						pathtomat += temp[i] + "\\";
				}
				pathtomat += tail(curline);

#ifdef MOON_DEBUG_MODE
				std::cout << "- find materials in: " << pathtomat << std::endl;
#endif

				// Load Materials
				LoadMaterials(pathtomat);
			}
		}

		// Deal with last mesh
		if (!Indices.empty() && !Vertices.empty()) {
			// Create Mesh
			tempMesh = new Mesh(meshname, Vertices, Indices);

			// Insert Mesh
			LoadedMeshes.push_back(tempMesh);
		}

		file.close();

		// Set Materials for each Mesh
		OBJLoader::info = "Match Material And Mesh...";
		std::cout << "Match Material And Mesh..." << MeshMatNames.size() << std::endl;
		for (int i = 0; i < MeshMatNames.size(); i++) {
			std::string matname = MeshMatNames[i];

			std::cout << "searching material: " << matname << std::endl;
			// Find corresponding material name in loaded materials
			// when found copy material variables into mesh material
			Material* searchMat = MOON_MaterialManager::GetItem(matname);
			if (searchMat != NULL) {
				std::cout << "material founded: " << searchMat->name << std::endl;
				LoadedMeshes[i]->material = searchMat;
			}
		}

		OBJLoader::progress = 1.0f;
		OBJLoader::info = "Done...";
		if (LoadedMeshes.empty() && LoadedVertices.empty() && LoadedIndices.empty()) return false;
		else {
			for (auto &mesh : LoadedMeshes) {
				if (!mesh->material) mesh->material = MOON_MaterialManager::defaultMat;
#ifdef MOON_DEBUG_MODE
				std::cout << "\r- " << mesh->name << "| vertices > " << mesh->vertices.size()
						<< "| triangles > " << (mesh->triangles.size() / 3)
						<< "| texcoords > " << TCoords.size()
						<< "| normals > " << Normals.size();
				if (mesh->material)
					std::cout << "| material: " + mesh->material->name << std::endl;
#endif
			}
			return true;
		}
	}

	void OBJLoader::GenVerticesFromRawOBJ(std::vector<Vertex>& oVerts,
		const std::vector<Vector3>& iPositions,
		const std::vector<Vector2>& iTCoords,
		const std::vector<Vector3>& iNormals,
		std::string icurline) {
		std::vector<std::string> sface, svert;
		Vertex vVert;
		split(tail(icurline), sface, " ");

		bool noNormal = false;

		// For every given vertex do this
		for (int i = 0; i < int(sface.size()); i++) {
			// See What type the vertex is.
			int vtype;
			split(sface[i], svert, "/");

			// Check for just position - v1
			if (svert.size() == 1) // Only position
				vtype = 1;

			// Check for position & texture - v1/vt1
			if (svert.size() == 2) // Position & Texture
				vtype = 2;

			// Check for Position, Texture and Normal - v1/vt1/vn1
			// or if Position and Normal - v1//vn1
			if (svert.size() == 3) {
				if (svert[1] != "") // Position, Texture, and Normal
					vtype = 4;
				else // Position & Normal
					vtype = 3;
			}

			// Calculate and store the vertex
			switch (vtype) {
				case 1: { // P
					vVert.Position = getElement(iPositions, svert[0]);
					vVert.UV = Vector2(0, 0);
					noNormal = true;
					oVerts.push_back(vVert);
					break;
				}
				case 2: { // P/T
					vVert.Position = getElement(iPositions, svert[0]);
					vVert.UV = getElement(iTCoords, svert[1]);
					noNormal = true;
					oVerts.push_back(vVert);
					break;
				}
				case 3: { // P//N
					vVert.Position = getElement(iPositions, svert[0]);
					vVert.UV = Vector2(0, 0);
					vVert.Normal = getElement(iNormals, svert[2]);
					oVerts.push_back(vVert);
					break;
				}
				case 4: { // P/T/N
					vVert.Position = getElement(iPositions, svert[0]);
					vVert.UV = getElement(iTCoords, svert[1]);
					vVert.Normal = getElement(iNormals, svert[2]);
					oVerts.push_back(vVert);
					break;
				}
				default: {
					break;
				}
			}
		}

		// take care of missing normals,
		// these may not be truly accurate but it is the 
		// best they get for not compiling a mesh with normals	
		if (noNormal) {
			Vector3 A = oVerts[0].Position - oVerts[1].Position;
			Vector3 B = oVerts[2].Position - oVerts[1].Position;

			Vector3 normal = Vector3::Cross(A, B);
			for (int i = 0; i < int(oVerts.size()); i++) {
				oVerts[i].Normal = normal;
			}
		}
	}

	// Triangulate a list of vertices into a face by printing inducies corresponding with triangles within it
	void OBJLoader::VertexTriangluation(std::vector<unsigned int>& oIndices, const std::vector<Vertex>& iVerts) {
		// If there are 2 or less verts, no triangle can be created, so exit
		if (iVerts.size() < 3) return;

		// If it is a triangle no need to calculate it
		if (iVerts.size() == 3) {
			oIndices.push_back(0);
			oIndices.push_back(1);
			oIndices.push_back(2);
			return;
		}

		// Create a list of vertices
		std::vector<Vertex> tVerts = iVerts;

		while (true) {
			// For every vertex
			for (int i = 0; i < int(tVerts.size()); i++) {
				// pPrev = the previous vertex in the list
				Vertex pPrev;
				if (i == 0) pPrev = tVerts[tVerts.size() - 1];
				else pPrev = tVerts[i - 1];

				// pCur = the current vertex;
				Vertex pCur = tVerts[i];

				// pNext = the next vertex in the list
				Vertex pNext;
				if (i == tVerts.size() - 1) pNext = tVerts[0];
				else pNext = tVerts[i + 1];


				// Check to see if there are only 3 verts left, if so this is the last triangle
				if (tVerts.size() == 3) {
					// Create a triangle from pCur, pPrev, pNext
					for (int j = 0; j < int(tVerts.size()); j++) {
						if (iVerts[j].Position == pCur.Position)
							oIndices.push_back(j);
						if (iVerts[j].Position == pPrev.Position)
							oIndices.push_back(j);
						if (iVerts[j].Position == pNext.Position)
							oIndices.push_back(j);
					}

					tVerts.clear();
					break;
				}
				if (tVerts.size() == 4) {
					// Create a triangle from pCur, pPrev, pNext
					for (int j = 0; j < int(iVerts.size()); j++) {
						if (iVerts[j].Position == pCur.Position)
							oIndices.push_back(j);
						if (iVerts[j].Position == pPrev.Position)
							oIndices.push_back(j);
						if (iVerts[j].Position == pNext.Position)
							oIndices.push_back(j);
					}

					Vector3 tempVec;
					for (int j = 0; j < int(tVerts.size()); j++) {
						if (tVerts[j].Position != pCur.Position
							&& tVerts[j].Position != pPrev.Position
							&& tVerts[j].Position != pNext.Position) {
							tempVec = tVerts[j].Position;
							break;
						}
					}

					// Create a triangle from pCur, pPrev, pNext
					for (int j = 0; j < int(iVerts.size()); j++) {
						if (iVerts[j].Position == pPrev.Position)
							oIndices.push_back(j);
						if (iVerts[j].Position == pNext.Position)
							oIndices.push_back(j);
						if (iVerts[j].Position == tempVec)
							oIndices.push_back(j);
					}

					tVerts.clear();
					break;
				}

				// If Vertex is not an interior vertex
				float angle = Vector3::Angle(pPrev.Position - pCur.Position, pNext.Position - pCur.Position) * (180 / 3.14159265359);
				if (angle <= 0 && angle >= 180) continue;

				// If any vertices are within this triangle
				bool inTri = false;
				for (int j = 0; j < int(iVerts.size()); j++) {
					if (MoonMath::IsInTriangle(iVerts[j].Position, pPrev.Position, pCur.Position, pNext.Position)
						&& iVerts[j].Position != pPrev.Position
						&& iVerts[j].Position != pCur.Position
						&& iVerts[j].Position != pNext.Position) {
						inTri = true;
						break;
					}
				}
				if (inTri) continue;

				// Create a triangle from pCur, pPrev, pNext
				for (int j = 0; j < int(iVerts.size()); j++) {
					if (iVerts[j].Position == pCur.Position)
						oIndices.push_back(j);
					if (iVerts[j].Position == pPrev.Position)
						oIndices.push_back(j);
					if (iVerts[j].Position == pNext.Position)
						oIndices.push_back(j);
				}

				// Delete pCur from the list
				for (int j = 0; j < int(tVerts.size()); j++) {
					if (tVerts[j].Position == pCur.Position) {
						tVerts.erase(tVerts.begin() + j);
						break;
					}
				}

				// reset i to the start -1 since loop will add 1 to it
				i = -1;
			}

			// if no triangles were created
			if (oIndices.size() == 0) break;

			// if no more vertices
			if (tVerts.size() == 0) break;
		}
	}

	bool OBJLoader::LoadMaterials(const std::string &path) {
		// If the file is not a material file return false
		if (path.substr(path.size() - 4, path.size()) != ".mtl")
			return false;

		std::ifstream file(path);
		// If the file is not found return false
		if (!file.is_open()) return false;
		std::cout << "mtl opened... ..." << std::endl;

		Material* tempMaterial = NULL;
		// Go through each line looking for material variables
		std::string curline;
		while (std::getline(file, curline)) {
			// new material and material name
			if (firstToken(curline) == "newmtl") {
				std::string matName = "none";
				if (curline.size() > 7) matName = tail(curline);
				tempMaterial = MOON_MaterialManager::CreateMaterial("MoonMtl", matName);
			}
			
			if (firstToken(curline) == "Ka") { // Ambient Color
				std::vector<std::string> temp;
				split(tail(curline), temp, " ");

				if (temp.size() != 3) continue;

				/*dynamic_cast<MoonMtl*>(tempMaterial)->ambientC.setValue(
					std::stof(temp[0]), std::stof(temp[1]), std::stof(temp[2])
				);*/
			} else if (firstToken(curline) == "Kd") { // Diffuse Color
				std::vector<std::string> temp;
				split(tail(curline), temp, " ");
				if (temp.size() != 3) continue;

				dynamic_cast<MoonMtl*>(tempMaterial)->diffuseC.setValue(
					std::stof(temp[0]), std::stof(temp[1]), std::stof(temp[2])
				);
			} else if (firstToken(curline) == "Ks") { // Specular Color
				std::vector<std::string> temp;
				split(tail(curline), temp, " ");
				if (temp.size() != 3) continue;

				dynamic_cast<MoonMtl*>(tempMaterial)->reflectW.setValue(
					std::stof(temp[0]), std::stof(temp[1]), std::stof(temp[2])
				);
			} else if (firstToken(curline) == "Ns") { // Specular Exponent
				//dynamic_cast<MoonMtl*>(tempMaterial)->glossiness = std::stof(tail(curline));
			} else if (firstToken(curline) == "Ni") { // Optical Density
				dynamic_cast<MoonMtl*>(tempMaterial)->refractW = Vector3::ONE() * std::stof(tail(curline));
			} else if (firstToken(curline) == "d") {  // Dissolve
				dynamic_cast<MoonMtl*>(tempMaterial)->translucency = Vector3::ONE() * std::stof(tail(curline));
			} else if (firstToken(curline) == "illum") { // Illumination
				dynamic_cast<MoonMtl*>(tempMaterial)->illumination = Vector3::ONE() * std::stoi(tail(curline));
			}

			// Load textures
			if (firstToken(curline) == "map_Ka") {
				dynamic_cast<MoonMtl*>(tempMaterial)->textures.push_back(LoadTexture(tail(curline), "ambientMap"));
			} else if (firstToken(curline) == "map_Kd") {
				dynamic_cast<MoonMtl*>(tempMaterial)->textures.push_back(LoadTexture(tail(curline), "diffuseMap"));
			} else if (firstToken(curline) == "map_Ks") {
				dynamic_cast<MoonMtl*>(tempMaterial)->textures.push_back(LoadTexture(tail(curline), "reflectMap"));
			} else if (firstToken(curline) == "map_Ns") {
				dynamic_cast<MoonMtl*>(tempMaterial)->textures.push_back(LoadTexture(tail(curline), "glossinessMap"));
			} else if (firstToken(curline) == "map_d") {
				dynamic_cast<MoonMtl*>(tempMaterial)->textures.push_back(LoadTexture(tail(curline), "alphaMap"));
			} else if (firstToken(curline) == "map_Bump" || firstToken(curline) == "map_bump" || firstToken(curline) == "bump") {
				dynamic_cast<MoonMtl*>(tempMaterial)->textures.push_back(LoadTexture(tail(curline), "normalMap"));
			}
		}

		// Test to see if anything was loaded
		//if (LoadedMaterials.empty()) return false;
		//else return true;
	}

	Texture* OBJLoader::LoadTexture(const std::string &path, const std::string &name) {
		// if find corresponding texture in loaded textures
		Texture* searchTex = MOON_TextureManager::GetItem(Utility::GetPathOrURLShortName(path));
		if (searchTex != NULL) return searchTex;

		// else load that new texture
		GLuint texID = (GLuint)MOON_UNSPECIFIEDID;
		Texture* newTex = MOON_TextureManager::LoadTexture(path, name);

		return newTex;
	}
}