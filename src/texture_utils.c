#include "basileus/texture_utils.h"

#include <glad/gl.h>
#include "stb_image.h"


unsigned int load_texture(char *file_path) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, num_channels;
    unsigned char *data = stbi_load(file_path, &width, &height, &num_channels, 0);

    // Assign the correct image format type based on number of image channels
    GLenum format;
    if (num_channels == 3) {
        format = GL_RGB;
    }
    else if (num_channels == 4) {
        format = GL_RGBA;
    }
    else {
        format = GL_RED;
    }

    // Actually create the texture that OpenGL can read
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        fprintf(stderr, "ERROR: Failed to load texture!");
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);
    return texture;
}

