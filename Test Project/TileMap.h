#ifndef _TILEMAP_H_
#define _TILEMAP_H_

#include "System.h"

#include <vector>

namespace Graphics
{
	

	template <typename T>
	class TileMap
	{
	private:
		struct MapData
		{
			unsigned int uiRow, uiCollumn;
		};
		std::vector<GLSurface<T>*> m_vglTileSurfaces;

		std::vector<std::vector<MapData>> m_voMapData;

	public:
		TileMap(const char* ac_szFilename, const char* ac_szImageName, const System::Size2D<T> ac_TileSize, const System::Size2D<T> ac_MapDimensions, const System::Point2D<T> ac_OriginPos);
		TileMap() = delete;
		~TileMap();
	};
}

namespace Graphics
{
	template <typename T>
	TileMap<T>::TileMap(const char* ac_szFilename, const char* ac_szImageName, const System::Size2D<T> ac_TileSize, const System::Size2D<T> ac_MapDimensions, const System::Point2D<T> ac_OriginPos)
	{
		FILE *File; // The text file for the tile set
		fopen_s(&File, ac_szFilename, "r"); // Loading the text file for the tile set

		if (File != NULL)
		{
			MapData oTemp = { NULL, NULL };
			std::vector<MapData> voTemp;

			for (int y = 0; y < ac_MapDimensions.H; ++y)
			{
				for (int x = 0; x < ac_MapDimensions.W; ++x)
				{
					fscanf_s(File, "%d:%d ", &oTemp.uiRow, &oTemp.uiCollumn);
					voTemp.push_back(oTemp);

					GLSurface<int>* glTemp = LoadSurface<int>("Images/environment.png");
					glTemp->OffsetP = { (int)oTemp.uiRow*ac_TileSize.W, (int)oTemp.uiCollumn*ac_TileSize.H };
					glTemp->OffsetD = { ac_TileSize.W, ac_TileSize.H };
					glTemp->Pos = { (x*ac_TileSize.W) + ac_OriginPos.X, (y*ac_TileSize.H) + ac_OriginPos.Y };
					glTemp->Layer = LayerType::BACKGROUND;

					m_vglTileSurfaces.push_back(glTemp);
				}
				fscanf_s(File, "\n");

				m_voMapData.push_back(voTemp);
				voTemp.clear();
			}
			fclose(File);
		}
		else
			printf("Error opening \"%s\" make sure the file exists...", ac_szFilename);
	}
	template <typename T>
	TileMap<T>::~TileMap()
	{

	}
}

#endif _TILEMAP_H_