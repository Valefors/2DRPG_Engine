#include <iostream>

#include <GL/gl3w.h>

#include "graphics.h"

void init_graphics()
{
	// gl3w initialization before any OpenGL operation
    if (gl3wInit())
    {
        std::cout << "Failed to initialize gl3w.\n";
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "gl3w initialized successfully.\n";
    }

    if (!gl3wIsSupported(3, 3))
    {
        std::cout << "Your Graphic Card doesn't support OpenGL 3.3.\nYour Graphic Card supports OpenGL " << glGetString(GL_VERSION) << '\n';
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "Your Graphic Card supports OpenGL 3.3.\n";
    }

    // Active transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static const float quad_VBO[24] =
{
    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f
};

void draw_triangle()
{
    unsigned int VAO; //Structure de données contient les informations servant à dessiner un VBO
    glGenVertexArrays(1, &VAO); //Alloue un VAO dans la VRAM

    unsigned int VBO; //Espace mémoire dans la VRAM
    glGenBuffers(1, &VBO); //Alloue un VBO dans la VRAM

    glBindVertexArray(VAO); //Indication du VAO en cours d'édition pour les fonctions futures OpenGL

    glBindBuffer(GL_ARRAY_BUFFER, VBO); //Indication du VBO en cours d'édition pour les fonctions futures OpenGL
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, quad_VBO, GL_STATIC_DRAW); //Remplit le VBO en cours d'édition du contenu du tableau passé en paramètre (quad_VBO)

    glEnableVertexAttribArray(0); //Active le premier channel (coordonnées espace) du VAO en cours d'édition
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); //Description du premier channel 0, contient 2 éléments de type GL_FLOAT, les infos d'un vertex font 4 float de taille. Commence au début du tableau

    glEnableVertexAttribArray(1); //Active le deuxième channel (coordonnées texture) du VAO en cours d'édition
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))); //Description du premier channel 1, contient 2 éléments de type GL_FLOAT, les infos d'un vertex font 4 float de taille. Commence avec 2 float de décalage

    glBindVertexArray(0); //Ne travaille plus sur aucun VAO
}