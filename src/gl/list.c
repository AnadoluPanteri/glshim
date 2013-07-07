#include "gl.h"
#include "list.h"

#define alloc_sublist(n, cap) \
    (GLfloat *)malloc(n * sizeof(GLfloat) * cap)

#define realloc_sublist(ref, n, cap) \
    if (ref)                         \
        ref = (GLfloat *)realloc(ref, n * sizeof(GLfloat) * cap)

RenderList *alloc_renderlist() {
    RenderList *list = (RenderList *)malloc(sizeof(RenderList));
    list->len = 0;
    list->cap = DEFAULT_RENDER_LIST_CAPACITY;

    list->calls.len = 0;
    list->calls.cap = 0;
    list->calls.calls = NULL;

    list->mode = 0;
    list->vert = NULL;
    list->normal = NULL;
    list->color = NULL;
    list->tex = NULL;
    list->material = NULL;

    list->next = NULL;
    return list;
}

RenderList *extend_renderlist(RenderList *list) {
    list->next = alloc_renderlist();
    return list->next;
}

void free_renderlist(RenderList *list) {
    RenderList *next;
    do {
        if (list->calls.len > 0) {
            for (int i = 0; i < list->calls.len; i++) {
                free(list->calls.calls[i]);
            }
            free(list->calls.calls);
        }
        if (list->vert) free(list->vert);
        if (list->normal) free(list->normal);
        if (list->color) free(list->color);
        if (list->tex) free(list->tex);
        if (list->material) free(list->material);
        next = list->next;
        free(list);
    } while ((list = next));
}

static inline
void resize_renderlist(RenderList *list) {
    if (list->len >= list->cap) {
        list->cap += DEFAULT_RENDER_LIST_CAPACITY;
        realloc_sublist(list->vert, 3, list->cap);
        realloc_sublist(list->normal, 3, list->cap);
        realloc_sublist(list->color, 4, list->cap);
        realloc_sublist(list->tex, 2, list->cap);
    }
}

void q2t_renderlist(RenderList *list) {
    // TODO: this has too much duplicate code
    if (!list->len || !list->vert) return;
    GLfloat *newVertPos, *vertPos, *vert;
    GLfloat *newNormalPos, *normalPos, *normal;
    GLfloat *newColorPos, *colorPos, *color;
    GLfloat *newTexPos, *texPos, *tex;

    vertPos = vert = list->vert;
    normalPos = normal = list->normal;
    colorPos = color = list->color;
    texPos = tex = list->tex;

    int newLen = list->len * 1.5;
    if (vert) newVertPos = list->vert = alloc_sublist(3, newLen);
    if (normal) newNormalPos = list->normal = alloc_sublist(3, newLen);
    if (color) newColorPos = list->color = alloc_sublist(4, newLen);
    if (tex) newTexPos = list->tex = alloc_sublist(2, newLen);;

    // GLfloat *a, *b, *c, *d;
    int a = 0, b = 1, c = 2, d = 3;
    int v1, v2, v3, v4, v5, v6;

    for (int i = 0; i < list->len; i += 4) {
        /*
        a = &vertPos[3*0];
        b = &vertPos[3*1];
        c = &vertPos[3*2];
        d = &vertPos[3*3];
        #define dist(a, b)\
            pow(a[0]-b[0], 2) + pow(a[1]-b[1], 2) + pow(a[2]-b[2], 2)
        if (dist(a, c) < dist(b, d)) {
            v3 = 2; v4 = 0; v5 = 2; v6 = 3;
        } else {
            v3 = 3; v4 = 3; v5 = 1; v6 = 2;
        }
        #undef dist
        */
        v1 = a; v2 = b; v3 = d;
        v4 = b; v5 = c; v6 = d;

        memcpy(newVertPos,      &vertPos[3*v1], sizeof(GLfloat) * 3);
        memcpy(newVertPos += 3, &vertPos[3*v2], sizeof(GLfloat) * 3);
        memcpy(newVertPos += 3, &vertPos[3*v3], sizeof(GLfloat) * 3);
        memcpy(newVertPos += 3, &vertPos[3*v4], sizeof(GLfloat) * 3);
        memcpy(newVertPos += 3, &vertPos[3*v5], sizeof(GLfloat) * 3);
        memcpy(newVertPos += 3, &vertPos[3*v6], sizeof(GLfloat) * 3);
        newVertPos += 3;
        vertPos += 4 * 3;
        if (normal) {
            memcpy(newNormalPos,      &normalPos[3*v1], sizeof(GLfloat) * 3);
            memcpy(newNormalPos += 3, &normalPos[3*v2], sizeof(GLfloat) * 3);
            memcpy(newNormalPos += 3, &normalPos[3*v3], sizeof(GLfloat) * 3);
            memcpy(newNormalPos += 3, &normalPos[3*v4], sizeof(GLfloat) * 3);
            memcpy(newNormalPos += 3, &normalPos[3*v5], sizeof(GLfloat) * 3);
            memcpy(newNormalPos += 3, &normalPos[3*v6], sizeof(GLfloat) * 3);
            newNormalPos += 3;
            normalPos += 4 * 3;
        }
        if (color) {
            memcpy(newColorPos,      &colorPos[4*v1], sizeof(GLfloat) * 4);
            memcpy(newColorPos += 4, &colorPos[4*v2], sizeof(GLfloat) * 4);
            memcpy(newColorPos += 4, &colorPos[4*v3], sizeof(GLfloat) * 4);
            memcpy(newColorPos += 4, &colorPos[4*v4], sizeof(GLfloat) * 4);
            memcpy(newColorPos += 4, &colorPos[4*v5], sizeof(GLfloat) * 4);
            memcpy(newColorPos += 4, &colorPos[4*v6], sizeof(GLfloat) * 4);
            newColorPos += 4;
            colorPos += 4 * 4;
        }
        if (tex) {
            memcpy(newTexPos,      &texPos[2*v1], sizeof(GLfloat) * 2);
            memcpy(newTexPos += 2, &texPos[2*v2], sizeof(GLfloat) * 2);
            memcpy(newTexPos += 2, &texPos[2*v3], sizeof(GLfloat) * 2);
            memcpy(newTexPos += 2, &texPos[2*v4], sizeof(GLfloat) * 2);
            memcpy(newTexPos += 2, &texPos[2*v5], sizeof(GLfloat) * 2);
            memcpy(newTexPos += 2, &texPos[2*v6], sizeof(GLfloat) * 2);
            newTexPos += 2;
            texPos += 4 * 2;
        }
    }
    list->len = list->cap = newLen;

    if (vert) free(vert);
    if (normal) free(normal);
    if (color) free(color);
    if (tex) free(tex);
}

static void wrap_tex_coords(RenderList *list) {
    if (list->tex) {
        GLfloat *tex = list->tex;
        GLfloat bot[] = {0, 0};
        for (int i = 0; i < list->len * 2; i += 2) {
            bot[0] = fmin(tex[i], bot[0]);
            bot[1] = fmin(tex[i+1], bot[1]);
        }
        bot[0] = floor(bot[0]);
        bot[1] = floor(bot[1]);
        if (bot[0] > 0 && bot[1] > 0) {
            for (int i = 0; i < list->len * 2; i += 2) {
                tex[i] -= bot[0];
                tex[i+1] -= bot[1];
            }
        }
    }
}

void end_renderlist(RenderList *list) {
    switch (list->mode) {
        case GL_QUADS:
            list->mode = GL_TRIANGLES;
            q2t_renderlist(list);
            break;
        case GL_POLYGON:
            list->mode = GL_TRIANGLE_FAN;
            break;
        case GL_QUAD_STRIP:
            list->mode = GL_TRIANGLE_STRIP;
            break;
    }
    if (list->tex) {
        wrap_tex_coords(list);
    }
}

void draw_renderlist(RenderList *list) {
    if (!list) return;
    LOAD_GLES(void, glDrawArrays, GLenum, GLint, GLsizei);

    glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
    do {
        // optimize zero-length segments out earlier?
        CallList *cl = &list->calls;
        if (cl->len > 0) {
            for (int i = 0; i < cl->len; i++) {
                glPackedCall(cl->calls[i]);
            }
        }
        if (! list->len)
            continue;

        if (list->vert) {
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, 0, list->vert);
        } else {
            glDisableClientState(GL_VERTEX_ARRAY);
        }

        if (list->normal) {
            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_FLOAT, 0, list->normal);
        } else {
            glDisableClientState(GL_NORMAL_ARRAY);
        }

        if (list->color) {
            glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(4, GL_FLOAT, 0, list->color);
        } else {
            glDisableClientState(GL_COLOR_ARRAY);
        }

        if (list->material) {
            RenderMaterial *m = list->material;
            int i, pos = 0;
            for (i = 0; i < list->material->count; i++) {
                m = list->material + i;
                glMaterialfv(GL_FRONT_AND_BACK, m->pname, m->color);
            }
        }

        GLfloat *tex = list->tex;
        GLuint texture;

        bool stipple = false;
        if (! tex) {
            // TODO: do we need to support GL_LINE_STRIP?
            if (list->mode == GL_LINES && state.enable.line_stipple && false) {
                stipple = true;
                glPushAttrib(GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT);
                glEnable(GL_BLEND);
                glEnable(GL_TEXTURE_2D);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                texture = gen_stipple_tex(list->vert, &tex, list->len);
                // TODO: cache this for display list on first render?
            }

            if (state.enable.texgen_s || state.enable.texgen_t) {
                gen_tex_coords(list->vert, &tex, list->len);
            }
        }

        if (tex) {
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(2, GL_FLOAT, 0, tex);
        } else {
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }

        gles_glDrawArrays(list->mode, 0, list->len);
        if (stipple) {
            glDeleteTextures(1, &texture);
            glPopAttrib();
        }

        if (tex && tex != list->tex) {
            if (list->tex)
                free(list->tex);

            list->tex = tex;
        }
    } while ((list = list->next));
    glPopClientAttrib();
}

// gl function wrappers

void rlVertex3f(RenderList *list, GLfloat x, GLfloat y, GLfloat z) {
    if (list->vert == NULL) {
        list->vert = alloc_sublist(3, list->cap);
    } else {
        resize_renderlist(list);
    }

    if (list->normal) {
        GLfloat *normal = list->color + (list->len * 3);
        memcpy(normal, list->lastNormal, sizeof(GLfloat) * 3);
    }

    if (list->color) {
        GLfloat *color = list->color + (list->len * 4);
        memcpy(color, list->lastColor, sizeof(GLfloat) * 4);
    }

    if (list->tex) {
        GLfloat *tex = list->tex + (list->len * 2);
        memcpy(tex, list->lastTex, sizeof(GLfloat) * 2);
    }

    GLfloat *vert = list->vert + (list->len++ * 3);
    vert[0] = x; vert[1] = y; vert[2] = z;
}

void rlNormal3f(RenderList *list, GLfloat x, GLfloat y, GLfloat z) {
    GLfloat *normal = list->lastNormal;
    normal[0] = x; normal[1] = y; normal[2] = z;

    if (list->normal == NULL) {
        list->normal = alloc_sublist(3, list->cap);
        // catch up
        int i;
        for (i = 0; i < list->len; i++) {
            GLfloat *normal = (list->normal + (i * 4));
            memcpy(normal, list->lastNormal, sizeof(GLfloat) * 4);
        }
    } else {
        resize_renderlist(list);
    }
}

void rlColor4f(RenderList *list, GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    if (list->color == NULL) {
        list->color = alloc_sublist(4, list->cap);
        // catch up
        int i;
        for (i = 0; i < list->len; i++) {
            GLfloat *color = (list->color + (i * 4));
            memcpy(color, list->lastColor, sizeof(GLfloat) * 4);
        }
    } else {
        resize_renderlist(list);
    }

    GLfloat *color = list->lastColor;
    color[0] = r; color[1] = g; color[2] = b; color[3] = a;
}

void rlMaterialfv(RenderList *list, GLenum face, GLenum pname, const GLfloat * params) {
    int *count;
    if (! list->material) {
        // TODO: fixed size is a hack
        list->material = malloc(20 * sizeof(RenderMaterial));
        list->material->count = 0;
    }
    RenderMaterial *m = list->material + list->material->count;
    m->face = face;
    m->pname = pname;
    memcpy(m->color, params, sizeof(GLfloat) * 4);

    list->material->count++;
}

void rlTexCoord2f(RenderList *list, GLfloat s, GLfloat t) {
    GLfloat *tex = list->lastTex;
    tex[0] = s; tex[1] = t;

    if (list->tex == NULL) {
        list->tex = alloc_sublist(2, list->cap);
    } else {
        resize_renderlist(list);
    }
}

void rlPushCall(RenderList *list, UnknownCall *data) {
    CallList *cl = &list->calls;
    if (!cl->calls) {
        cl->cap = DEFAULT_CALL_LIST_CAPACITY;
        cl->calls = malloc(DEFAULT_CALL_LIST_CAPACITY * sizeof(uintptr_t));
    } else if (list->calls.len == list->calls.cap) {
        cl->cap += DEFAULT_CALL_LIST_CAPACITY;
        cl->calls = realloc(cl->calls, cl->cap * sizeof(uintptr_t));
    }
    cl->calls[cl->len++] = data;
}

#undef alloc_sublist
#undef realloc_sublist
