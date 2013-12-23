#ifndef MAIN_H
#define MAIN_H

#include <string>
#include "YafScene.h"

#define NTHEMES 4
static const char* THEMES[NTHEMES] = { "cequis_default.xml", "cequis_football.xml", "cequis_pokemon.xml", "cequis_world_cup.xml" };

YafScene* ParseYafFile(const std::string& file);

#endif
