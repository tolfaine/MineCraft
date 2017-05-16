#pragma once

#include "engine/render/renderer.h"
#include "cube.h"

/**
  * On utilise des chunks pour que si on modifie juste un cube, on ait pas
  * besoin de recharger toute la carte dans le buffer, mais juste le chunk en question
  */
class NYChunk
{
	public :

		static const int CHUNK_SIZE = 16; ///< Taille d'un chunk en nombre de cubes (n*n*n)
		static const int CHUNK_HEIGHT = 45;

		NYCube _Cubes[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE]; ///< Cubes contenus dans le chunk

		GLuint _BufWorld; ///< Identifiant du VBO pour le monde
		
		static float _WorldVert[CHUNK_SIZE*CHUNK_HEIGHT*CHUNK_SIZE * 3 * 4 * 6]; ///< Buffer pour les sommets
		static float _WorldCols[CHUNK_SIZE*CHUNK_HEIGHT*CHUNK_SIZE * 3 * 4 * 6]; ///< Buffer pour les couleurs
		static float _WorldNorm[CHUNK_SIZE*CHUNK_HEIGHT*CHUNK_SIZE * 3 * 4 * 6]; ///< Buffer pour les normales

		static const int SIZE_VERTICE = 3 * sizeof(float); ///< Taille en octets d'un vertex dans le VBO
		static const int SIZE_COLOR = 3 * sizeof(float);  ///< Taille d'une couleur dans le VBO
		static const int SIZE_NORMAL = 3 * sizeof(float);  ///< Taille d'une normale dans le VBO
		
		int _NbVertices; ///< Nombre de vertices dans le VBO (on ne met que les faces visibles)
		int _NbFloat;


		NYChunk * Voisins[6];
		
		NYChunk()
		{
			_NbFloat = 0;
			_NbVertices = 0;
			_BufWorld = 0;
			memset(Voisins,0x00,sizeof(void*) * 6);
		}

		void setVoisins(NYChunk * xprev, NYChunk * xnext,NYChunk * yprev,NYChunk * ynext,NYChunk * zprev,NYChunk * znext)
		{
			Voisins[0] = xprev;
			Voisins[1] = xnext;
			Voisins[2] = yprev;
			Voisins[3] = ynext;
			Voisins[4] = zprev;
			Voisins[5] = znext;
		}

		/**
		  * Raz de l'état des cubes (a draw = false)
		  */
		void reset(void)
		{
			for(int x=0;x<CHUNK_SIZE;x++)
				for (int y = 0; y<CHUNK_HEIGHT; y++)
					for(int z=0;z<CHUNK_SIZE;z++)
					{
						_Cubes[x][y][z]._Draw = true;
						_Cubes[x][y][z]._Type = CUBE_AIR;
					}
		}

		//On met le chunk ddans son VBO
		void toVbo(void)
		{
			_NbVertices = 0;
			_NbFloat = 0;

			//On le detruit si il existe deja
			if (_BufWorld != 0)
				glDeleteBuffers(1, &_BufWorld);

			//Genere un identifiant
			glGenBuffers(1, &_BufWorld);

			/*
			glColor3d(0.5, 0, 0);
			glVertex3d(1, 1, 1);
			glVertex3d(-1, -1, 1);
			glVertex3d(1, -1, 1);
			*/

			/*
			_WorldVert[_NbFloat] = 1.0;
			_NbFloat++;
			_WorldVert[_NbFloat] = 1.0;
			_NbFloat++;
			_WorldVert[_NbFloat] = 1.0;
			_NbFloat++;
			_NbVertices++;

			_WorldVert[_NbFloat] = -1.0;
			_NbFloat++;
			_WorldVert[_NbFloat] = -1.0;
			_NbFloat++;
			_WorldVert[_NbFloat] = 1.0;
			_NbFloat++;
			_NbVertices++;

			_WorldVert[_NbFloat] = 1.0;
			_NbFloat++;
			_WorldVert[_NbFloat] = -1.0;
			_NbFloat++;
			_WorldVert[_NbFloat] = 1.0;
			_NbFloat++;
			_NbVertices++;

			*/

			
			for (int x = 0; x<CHUNK_SIZE; x++)
				for (int y = 0; y<CHUNK_HEIGHT; y++)
			for (int z = 0; z<CHUNK_SIZE; z++)
			{
				if (_Cubes[x][y][z]._Draw ==true && _Cubes[x][y][z]._Type != CUBE_AIR){
					BlockData(x, y, z);
				}
			}

		
			

			//On attache le VBO pour pouvoir le modifier
			glBindBuffer(GL_ARRAY_BUFFER, _BufWorld);

			//On reserve la quantite totale de datas (creation de la zone memoire, mais sans passer les données)
			//Les tailles g_size* sont en octets, à vous de les calculer
			glBufferData(GL_ARRAY_BUFFER,
				_NbVertices * 12 +
				_NbVertices * 12 +
				_NbVertices * 12,
				NULL,
				GL_STREAM_DRAW);

			//Check error (la tester ensuite...)
			//GLenum error = glGetError();

			NYRenderer::checkGlError("glBufferData");

			//On copie les vertices
			glBufferSubData(GL_ARRAY_BUFFER,
				0, //Offset 0, on part du debut                        
				_NbVertices * 12, //Taille en octets des datas copiés
				_WorldVert);  //Datas          

			//Check error (la tester ensuite...)
			NYRenderer::checkGlError("glBufferData");

			//On copie les couleurs
			glBufferSubData(GL_ARRAY_BUFFER,
				_NbVertices * 12, //Offset : on se place après les vertices
				_NbVertices * 12, //On copie tout le buffer couleur : on donne donc sa taille
				_WorldCols);  //Pt sur le buffer couleur       

			//Check error (la tester ensuite...)
			NYRenderer::checkGlError("glBufferData");

			glBufferSubData(GL_ARRAY_BUFFER,
				_NbVertices * SIZE_VERTICE +
				_NbVertices * SIZE_COLOR,
				_NbVertices * SIZE_NORMAL,
				_WorldNorm);


			//On debind le buffer pour eviter une modif accidentelle par le reste du code
			glBindBuffer(GL_ARRAY_BUFFER, 0);

		}

		void render(void)
		{

			glEnable(GL_COLOR_MATERIAL);
			glEnable(GL_LIGHTING);

			//On bind le buuffer
			glBindBuffer(GL_ARRAY_BUFFER, _BufWorld);
			NYRenderer::checkGlError("glBindBuffer");

			//On active les datas que contiennent le VBO
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);

			//On place les pointeurs sur les datas, aux bons offsets
			glVertexPointer(3, GL_FLOAT, 0, (void*)(0));
			glColorPointer(3, GL_FLOAT, 0, (void*)(_NbVertices*SIZE_VERTICE));
			glNormalPointer(GL_FLOAT, 0, (void*)(_NbVertices*SIZE_VERTICE + _NbVertices*SIZE_COLOR));

			//On demande le dessin
			glDrawArrays(GL_TRIANGLES, 0, _NbVertices);

			//On cleane
			glDisableClientState(GL_COLOR_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
		}

		/**
		  * On verifie si le cube peut être vu
		  */
		bool test_hidden(int x, int y, int z)
		{
			NYCube * cubeXPrev = NULL; 
			NYCube * cubeXNext = NULL; 
			NYCube * cubeYPrev = NULL; 
			NYCube * cubeYNext = NULL; 
			NYCube * cubeZPrev = NULL; 
			NYCube * cubeZNext = NULL; 

			if(x == 0 && Voisins[0] != NULL)
				cubeXPrev = &(Voisins[0]->_Cubes[CHUNK_SIZE-1][y][z]);
			else if(x > 0)
				cubeXPrev = &(_Cubes[x-1][y][z]);

			if(x == CHUNK_SIZE-1 && Voisins[1] != NULL)
				cubeXNext = &(Voisins[1]->_Cubes[0][y][z]);
			else if(x < CHUNK_SIZE-1)
				cubeXNext = &(_Cubes[x+1][y][z]);

			if(y == 0 && Voisins[2] != NULL)
				cubeYPrev = &(Voisins[2]->_Cubes[x][CHUNK_HEIGHT - 1][z]);
			else if(y > 0)
				cubeYPrev = &(_Cubes[x][y-1][z]);

			if(y == CHUNK_SIZE-1 && Voisins[3] != NULL)
				cubeYNext = &(Voisins[3]->_Cubes[x][0][z]);
			else if(y < CHUNK_SIZE-1)
				cubeYNext = &(_Cubes[x][y+1][z]);

			if(z == 0 && Voisins[4] != NULL)
				cubeZPrev = &(Voisins[4]->_Cubes[x][y][CHUNK_SIZE-1]);
			else if(z > 0)
				cubeZPrev = &(_Cubes[x][y][z-1]);

			if(z == CHUNK_SIZE-1 && Voisins[5] != NULL)
				cubeZNext = &(Voisins[5]->_Cubes[x][y][0]);
			else if(z < CHUNK_SIZE-1)
				cubeZNext = &(_Cubes[x][y][z+1]);

			if( cubeXPrev == NULL || cubeXNext == NULL ||
				cubeYPrev == NULL || cubeYNext == NULL ||
				cubeZPrev == NULL || cubeZNext == NULL )
				return false;

			if( cubeXPrev->isSolid() == true && //droite
				cubeXNext->isSolid() == true && //gauche
				cubeYPrev->isSolid() == true && //haut
				cubeYNext->isSolid() == true && //bas
				cubeZPrev->isSolid() == true && //devant
				cubeZNext->isSolid() == true )  //derriere
				return true;
			return false;
		}

		void disableHiddenCubes(void)
		{
			for(int x=0;x<CHUNK_SIZE;x++)
				for (int y = 0; y<CHUNK_HEIGHT; y++)
					for(int z=0;z<CHUNK_SIZE;z++)
					{
						_Cubes[x][y][z]._Draw = true;
						if(test_hidden(x,y,z))
							_Cubes[x][y][z]._Draw = false;
					}
		}

		void BlockData(int x, int y, int z){

			//glBegin(GL_TRIANGLES);

			BlockTexture(x, y, z);

			FaceUp(x, y, z);
			FaceDown(x, y, z);
			FaceNorth(x, y, z); 
			FaceSouth(x, y, z);
			FaceEast(x, y, z);
			FaceWest(x, y, z); 

		}

		void BlockTexture(int x, int y, int z){

			float color[3];
			if (_Cubes[x][y][z]._Type == CUBE_EAU){
				color[0] = 80.0 / 255.0;
				color[1] = 190.0 / 255.0;
				color[2] = 255.0 / 255.0;
			}
			if (_Cubes[x][y][z]._Type == CUBE_TERRE){
				color[0] = 90.0 / 255.0;
				color[1] = 67.0 / 255.0;
				color[2] = 24.0 / 255.0;
			}
			if (_Cubes[x][y][z]._Type == CUBE_HERBE){
				color[0] = 75.0 / 255.0;
				color[1] = 155.0 / 255.0;
				color[2] = 57.0 / 255.0;
			}
			if (_Cubes[x][y][z]._Type == CUBE_MONTAIN){
				color[0] = 50.0 / 255.0;
				color[1] = 50.0 / 255.0;
				color[2] = 50.0 / 255.0;
			}
			if (_Cubes[x][y][z]._Type == CUBE_ICE){
				color[0] = 255.0 / 255.0;
				color[1] = 255.0 / 255.0;
				color[2] = 255.0 / 255.0;
			}
			if (_Cubes[x][y][z]._Type == CUBE_SAND){
				color[0] = 251.0 / 255.0;
				color[1] = 225.0 / 255.0;
				color[2] = 117.0 / 255.0;
			}

			color[0] += (rand() % 100) / 2000.0f;
			color[1] += (rand() % 100) / 2000.0f;
			color[2] += (rand() % 100) / 2000.0f;

			for (int i = 0; i < 6*6; i++){
				_WorldCols[_NbFloat + i*3] = color[0];
				_WorldCols[_NbFloat + 1 + i * 3] = color[1];
				_WorldCols[_NbFloat + 2 + i * 3] = color[2];
			}

		}

		void AddNormal(int x, int y, int z){

			for (int i = 0; i < 6; i++){
				_WorldNorm[_NbFloat + i*3] = x;
				_WorldNorm[_NbFloat + 1 + i * 3] = y;
				_WorldNorm[_NbFloat + 2 + i * 3] = z;
			}


		}

		void FaceUp(int x, int y, int z){
			// UP

		//	glNormal3f(0, 0, 1);
			AddNormal(0, 0, 1);

			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;
			_NbFloat++;

			_NbVertices++;  


			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;
			_NbFloat++;
			_NbVertices++;

			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;
			_NbFloat++;
			_NbVertices++;


			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;
			_NbFloat++;
			_NbVertices++;


			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE ;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;
			_NbFloat++;
			_NbVertices++;


			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;
			_NbFloat++;
			_NbVertices++;

		}
		void FaceDown(int x, int y, int z){
			// DOWN

		//	glNormal3f(0, 0, -1);

			/*
			glColor3d(1, 0, 0);
			glVertex3d(1, 1, -1);
			glVertex3d(1, -1, -1);
			glVertex3d(-1, -1, -1);
			*/

			AddNormal(1, 0, 0);
			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE;
			_NbFloat++;
			_NbVertices++;
			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE;
			_NbFloat++;
			_NbVertices++;
			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE;
			_NbFloat++;
			_NbVertices++;

			/*
			glColor3d(0.5, 0, 0);
			glVertex3d(1, 1, -1);
			glVertex3d(-1, -1, -1);
			glVertex3d(-1, 1, -1);
			*/

			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE;
			_NbFloat++;
			_NbVertices++;
			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = y  * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE;
			_NbFloat++;
			_NbVertices++;
			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE;
			_NbFloat++;
			_NbVertices++;


		}
		void FaceNorth(int x, int y, int z){
			//BACK

			//glNormal3f(-1, 0, 0);
			/*
			glColor3d(0, 1, 0);
			glVertex3d(-1, 1, 1);
			glVertex3d(-1, -1, -1);
			glVertex3d(-1, -1, 1);
			*/
			AddNormal(-1, 0, 0);

			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;
			_NbFloat++;
			_NbVertices++;
			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE;
			_NbFloat++;
			_NbVertices++;
			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;
			_NbFloat++;
			_NbVertices++;

			/*
			glColor3d(0, 0.5, 0);
			glVertex3d(-1, 1, 1);
			glVertex3d(-1, 1, -1);
			glVertex3d(-1, -1, -1);
			*/


			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;
			_NbFloat++;
			_NbVertices++;
			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE;
			_NbFloat++;
			_NbVertices++;
			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE;
			_NbFloat++;
			_NbVertices++;

		}
		void FaceSouth(int x, int y, int z){
			//FRONT

			//glNormal3f(1, 0, 0);

			/*
			glColor3d(0, 1, 0);
			glVertex3d(1, 1, 1);
			glVertex3d(1, -1, 1);
			glVertex3d(1, -1, -1);

			*/

			AddNormal(1, 0, 0);

			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;
			_NbFloat++;
			_NbVertices++;
			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;
			_NbFloat++;
			_NbVertices++;
			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE;
			_NbFloat++;
			_NbVertices++;

			/*
			glColor3d(0, 0.5, 0);
			glVertex3d(1, 1, 1);
			glVertex3d(1, -1, -1);
			glVertex3d(1, 1, -1);
			*/

			_WorldVert[_NbFloat] = x  * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;;
			_NbFloat++;
			_NbVertices++;
			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE;
			_NbFloat++;
			_NbVertices++;
			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE;
			_NbFloat++;
			_NbVertices++;
		}
		void FaceEast(int x, int y, int z){
			// RIGHT
			//glNormal3f(0, -1, 0);

			/*
			glColor3d(0, 0, 1);
			glVertex3d(1, -1, 1);
			glVertex3d(-1, -1, -1);
			glVertex3d(1, -1, -1);
			*/

			AddNormal(0, -1, 0);

			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;;
			_NbFloat++;
			_NbVertices++;
			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE;
			_NbFloat++;
			_NbVertices++;
			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE;
			_NbFloat++;
			_NbVertices++;

			/*
			glColor3d(0, 0, 0.5);
			glVertex3d(1, -1, 1);
			glVertex3d(-1, -1, 1);
			glVertex3d(-1, -1, -1);
			*/

			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;;
			_NbFloat++;
			_NbVertices++;
			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;;
			_NbFloat++;
			_NbVertices++;
			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE;
			_NbFloat++;
			_NbVertices++;
		}
		void FaceWest(int x, int y, int z){
			// LEFT
		//	glNormal3f(0, 1, 0);

			/*
			
			glColor3d(0, 0, 1);
			glVertex3d(1, 1, 1);
			glVertex3d(1, 1, -1);
			glVertex3d(-1, 1, -1);
		*/
			AddNormal(0, 1, 0);

			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;;
			_NbFloat++;
			_NbVertices++;
			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE; 
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE;
			_NbFloat++;
			_NbVertices++;
			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE;
			_NbFloat++;
			_NbVertices++;

			/*
			glColor3d(0, 0, 0.5);
			glVertex3d(1, 1, 1);
			glVertex3d(-1, 1, -1);
			glVertex3d(-1, 1, 1);
			*/

			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;;
			_NbFloat++;
			_NbVertices++;
			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE;
			_NbFloat++;
			_NbVertices++;
			_WorldVert[_NbFloat] = x * NYCube::CUBE_SIZE;
			_NbFloat++;
			_WorldVert[_NbFloat] = y * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;;
			_NbFloat++;
			_WorldVert[_NbFloat] = z * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE;;
			_NbFloat++;
			_NbVertices++;
		}



};