# VR-project

Il y a un sol et une boule.\
La boule c'est simple, c'est juste un boule avec un shader Phong secular.\
Le sol c'est un objet plat, avec une texture en damier (`/Projet/objects/floor/floor.png`), dans le fichier `main2.cpp`, il y a un paramètre `grid_size`, donnez lui la valeur $n$ et le sol sera un carré de $n$ objets plats mis côte-à-côtes, la caméra et l'objet seront positionnés au centre (si vous comprenez pas les calculs demandez-moi, y a qqes trucs dûs au fait que l'objet en (0, 0) n'a pas son coin placé aux coordonnées (0, 0) mais son centre (d'où la nécéssité de faire des calculs -1.0f), aussi la dimension d'un plan est de 2.0f)

TODO:
- Voir comment mettre plusieurs shaders pour un même objet  (sûrement devoir refactor la méthode `Draw` des `Model` car ils ne peuvent prendre que un `Shader` en paramètre)
- Ajouter la librairie Bullet pour tester la physique (https://github.com/THISISAGOODNAME/bulletOpenGL)
- Mettre la normal map sur le sol (il y a un fichier de normal map dans le dossier `/Project/objects/floor/floor_map.png`) (https://learnopengl.com/Advanced-Lighting/Normal-Mapping)
- Rendre le sol encore plus paramétrisable (dimension d'un objet du sol et normaliser les calculs)
