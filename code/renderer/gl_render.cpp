/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 *
 * Descent 3: Apex
 * Copyright (C) 2024 by Justin Marshall
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "tr_local.h"

// Turns on/off multitexture blending
void opengl_SetMultitextureBlendMode(bool state) {
  if (OpenGL_multitexture_state == state)
    return;
  OpenGL_multitexture_state = state;
#if (defined(_USE_OGL_ACTIVE_TEXTURES))
  if (state) {

    glActiveTextureARB(GL_TEXTURE1_ARB);
    glClientActiveTextureARB(GL_TEXTURE1_ARB);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_TEXTURE_2D);

    glActiveTextureARB(GL_TEXTURE0_ARB);
    glClientActiveTextureARB(GL_TEXTURE0_ARB);
    Last_texel_unit_set = 0;

  } else {

    glActiveTextureARB(GL_TEXTURE1_ARB);
    glClientActiveTextureARB(GL_TEXTURE1_ARB);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);

    glActiveTextureARB(GL_TEXTURE0_ARB);
    glClientActiveTextureARB(GL_TEXTURE0_ARB);
    Last_texel_unit_set = 0;
  }
#endif
}


// Takes nv vertices and draws the polygon defined by those vertices.  Uses bitmap "handle"
// as a texture
void opengl_DrawMultitexturePolygon3D(int handle, g3Point **p, int nv, int map_type) {
  g3Point *pnt;
  int i, fr, fg, fb;
  float alpha;
  vector *vertp;
  color_array *colorp;
  tex_array *texp, *texp2;

  float one_over_square_res = 1.0 / GameLightmaps[Overlay_map].square_res;
  float xscalar = (float)GameLightmaps[Overlay_map].width * one_over_square_res;
  float yscalar = (float)GameLightmaps[Overlay_map].height * one_over_square_res;

  ASSERT(nv < 100);

  if (OpenGL_state.cur_light_state == LS_NONE) {
    fr = GR_COLOR_RED(OpenGL_state.cur_color);
    fg = GR_COLOR_GREEN(OpenGL_state.cur_color);
    fb = GR_COLOR_BLUE(OpenGL_state.cur_color);
  }

  alpha = Alpha_multiplier * OpenGL_Alpha_factor;

  vertp = &GL_verts[0];
  texp = &GL_tex_coords[0];
  texp2 = &GL_tex_coords2[0];
  colorp = &GL_colors[0];

  // Specify our coordinates
  for (i = 0; i < nv; i++, vertp++, texp++, colorp++, texp2++) {
    pnt = p[i];
    ASSERT(pnt->p3_flags & PF_ORIGPOINT);

    if (OpenGL_state.cur_alpha_type & ATF_VERTEX)
      alpha = pnt->p3_a * Alpha_multiplier * OpenGL_Alpha_factor;

    // If we have a lighting model, apply the correct lighting!
    if (OpenGL_state.cur_light_state != LS_NONE) {
      // Do lighting based on intesity (MONO) or colored (RGB)
      if (OpenGL_state.cur_color_model == CM_MONO) {
        colorp->r = pnt->p3_l;
        colorp->g = pnt->p3_l;
        colorp->b = pnt->p3_l;
        colorp->a = alpha;
      } else {
        colorp->r = pnt->p3_r;
        colorp->g = pnt->p3_g;
        colorp->b = pnt->p3_b;
        colorp->a = alpha;
      }
    } else {
      colorp->r = 1;
      colorp->g = 1;
      colorp->b = 1;
      colorp->a = alpha;
    }

    if (OpenGL_editor_renderSelected) {
      colorp->r = 1;
      colorp->g = 0;
      colorp->b = 0;
      colorp->a = alpha;
    }

    /*
    // Texture this polygon!
    float texw=1.0/(pnt->p3_z+Z_bias);
    texp->s=pnt->p3_u*texw;
    texp->t=pnt->p3_v*texw;
    texp->r=0;
    texp->w=texw;

    texp2->s=pnt->p3_u2*xscalar*texw;
    texp2->t=pnt->p3_v2*yscalar*texw;
    texp2->r=0;
    texp2->w=texw;
    */
    texp->s = pnt->p3_u;
    texp->t = pnt->p3_v;
    texp->r = 0.0f;
    texp->w = 1.0f;

    texp2->s = pnt->p3_u2 * xscalar;
    texp2->t = pnt->p3_v2 * yscalar;
    texp2->r = 0.0f;
    texp2->w = 1.0f;

    // Finally, specify a vertex
    /*
    vertp->x=pnt->p3_sx+x_add;
    vertp->y=pnt->p3_sy+y_add;
    vertp->z = -max(0,min(1.0,1.0-(1.0/(pnt->p3_z+Z_bias))));
    */
    *vertp = pnt->p3_vecPreRot;
  }

  // make sure our bitmap is ready to be drawn
  opengl_MakeBitmapCurrent(handle, map_type, 0);
  opengl_MakeWrapTypeCurrent(handle, map_type, 0);

  // make sure our bitmap is ready to be drawn
  opengl_MakeBitmapCurrent(Overlay_map, MAP_TYPE_LIGHTMAP, 1);
  opengl_MakeWrapTypeCurrent(Overlay_map, MAP_TYPE_LIGHTMAP, 1);

  opengl_SetMultitextureBlendMode(true);

  // And draw!
  glDrawArrays(GL_POLYGON, 0, nv);

  OpenGL_polys_drawn++;
  OpenGL_verts_processed += nv;

  CHECK_ERROR(10)
}

void opengl_DrawFlatPolygon3D(g3Point **p, int nv) {
  float fr, fg, fb;
  int i;

  if (UseMultitexture) {
    opengl_SetMultitextureBlendMode(false);
  }

  float alpha = Alpha_multiplier * OpenGL_Alpha_factor;

  fr = GR_COLOR_RED(OpenGL_state.cur_color);
  fg = GR_COLOR_GREEN(OpenGL_state.cur_color);
  fb = GR_COLOR_BLUE(OpenGL_state.cur_color);
  fr /= 255.0;
  fg /= 255.0;
  fb /= 255.0;

  // And draw!
  glBegin(GL_POLYGON);
  for (i = 0; i < nv; i++) {
    g3Point *pnt = p[i];
    ASSERT(pnt->p3_flags & PF_ORIGPOINT);

    if (OpenGL_state.cur_alpha_type & ATF_VERTEX)
      alpha = pnt->p3_a * Alpha_multiplier * OpenGL_Alpha_factor;

    // If we have a lighting model, apply the correct lighting!
    if (OpenGL_state.cur_light_state != LS_NONE) {
      // Do lighting based on intesity (MONO) or colored (RGB)
      if (OpenGL_state.cur_color_model == CM_MONO)
        glColor4f(pnt->p3_l, pnt->p3_l, pnt->p3_l, alpha);
      else {
        glColor4f(pnt->p3_r, pnt->p3_g, pnt->p3_b, alpha);
      }

    } else {
      glColor4f(fr, fg, fb, alpha);
    }

    /*
    // Finally, specify a vertex
    float z = max(0,min(1.0,1.0-(1.0/(pnt->p3_z+Z_bias))));
    glVertex3f (pnt->p3_sx+x_add,pnt->p3_sy+y_add,-z);
    */
    glVertex3f(pnt->p3_vecPreRot.x, pnt->p3_vecPreRot.y, pnt->p3_vecPreRot.z);
  }

  glEnd();
  CHECK_ERROR(11)
  OpenGL_polys_drawn++;
  OpenGL_verts_processed += nv;
}

// Takes nv vertices and draws the 3D polygon defined by those vertices.
// Uses bitmap "handle" as a texture
void rend_DrawPolygon3D(int handle, g3Point **p, int nv, int map_type) {
  g3Point *pnt;
  int i;
  float fr, fg, fb;
  float alpha;
  vector *vertp;
  color_array *colorp;
  tex_array *texp;

  ASSERT(nv < 100);

  if (OpenGL_state.cur_texture_quality == 0) {
    d3HardwareShaderScopedBind scopedShaderBind(shaderGeneric);
    g3_RefreshTransforms(false);
    opengl_DrawFlatPolygon3D(p, nv);    
    return;
  }

  if (Overlay_type != OT_NONE && UseMultitexture) {
    d3HardwareShaderScopedBind scopedShaderBind(shaderGenericLightmap);
    g3_RefreshTransforms(false);
    opengl_DrawMultitexturePolygon3D(handle, p, nv, map_type);
    return;
  }

  d3HardwareShaderScopedBind scopedShaderBind(shaderGeneric);
  g3_RefreshTransforms(false);

  if (OpenGL_state.cur_light_state == LS_FLAT_GOURAUD) {
    fr = GR_COLOR_RED(OpenGL_state.cur_color) / 255.0;
    fg = GR_COLOR_GREEN(OpenGL_state.cur_color) / 255.0;
    fb = GR_COLOR_BLUE(OpenGL_state.cur_color) / 255.0;
  }

  if (UseMultitexture) {
    opengl_SetMultitextureBlendMode(false);
  }

  // make sure our bitmap is ready to be drawn
  opengl_MakeBitmapCurrent(handle, map_type, 0);
  opengl_MakeWrapTypeCurrent(handle, map_type, 0);

  alpha = Alpha_multiplier * OpenGL_Alpha_factor;

  vertp = &GL_verts[0];
  texp = &GL_tex_coords[0];
  colorp = &GL_colors[0];

  // Specify our coordinates
  for (i = 0; i < nv; i++, vertp++, texp++, colorp++) {
    pnt = p[i];

    // all points should be original
    ASSERT(pnt->p3_flags & PF_ORIGPOINT);

    ////////////////////////////////////////////
    if (pnt->p3_flags & PF_ORIGPOINT) {
      if (!(pnt->p3_flags & PF_PROJECTED)) {
        g3_ProjectPoint(pnt);
      }

      // get the original point
      float origPoint[4];
      origPoint[0] = pnt->p3_vecPreRot.x;
      origPoint[1] = pnt->p3_vecPreRot.y;
      origPoint[2] = pnt->p3_vecPreRot.z;
      origPoint[3] = 1.0f;

      // transform by the full transform
      float view[4];
      g3_TransformVert(view, origPoint, gTransformFull);

      vector tempv = pnt->p3_vecPreRot - View_position;
      vector testPt = tempv * Unscaled_matrix;

      float screenX = pnt->p3_sx + OpenGL_state.clip_x1;
      float screenY = pnt->p3_sy + OpenGL_state.clip_y1;

      // normalize
      float oOW = 1.0f / view[3];
      view[0] *= oOW;
      view[1] *= oOW;
      view[2] *= oOW;

      oOW *= 1.0f;
    }
    ////////////////////////////////////////////

    if (OpenGL_state.cur_alpha_type & ATF_VERTEX) {
      alpha = pnt->p3_a * Alpha_multiplier * OpenGL_Alpha_factor;
    }

    // If we have a lighting model, apply the correct lighting!
    if (OpenGL_state.cur_light_state != LS_NONE) {
      if (OpenGL_state.cur_light_state == LS_FLAT_GOURAUD) {
        colorp->r = fr;
        colorp->g = fg;
        colorp->b = fb;
        colorp->a = alpha;
      } else {
        // Do lighting based on intesity (MONO) or colored (RGB)
        if (OpenGL_state.cur_color_model == CM_MONO) {
          colorp->r = pnt->p3_l;
          colorp->g = pnt->p3_l;
          colorp->b = pnt->p3_l;
          colorp->a = alpha;
        } else {
          colorp->r = pnt->p3_r;
          colorp->g = pnt->p3_g;
          colorp->b = pnt->p3_b;
          colorp->a = alpha;
        }
      }
    } else {
      colorp->r = 1;
      colorp->g = 1;
      colorp->b = 1;
      colorp->a = alpha;
    }

    if (OpenGL_editor_renderSelected) {
      colorp->r = 1;
      colorp->g = 0;
      colorp->b = 0;
      colorp->a = alpha;
    }

    /*
#ifdef __LINUX__
    //MY TEST HACK...MAYBE BAD DRIVERS? OR MAYBE THIS IS
    //HOW IT SHOULD BE DONE (STILL BUGGY)
    // Texture this polygon!
    float texw=1.0/(pnt->p3_z+Z_bias);
    if(OpenGL_TextureHack)
    {
            texp->s=pnt->p3_u;
            texp->t=pnt->p3_v;
    }else
    {
            texp->s=pnt->p3_u*texw;
            texp->t=pnt->p3_v*texw;
    }
    texp->r=0;
    texp->w=texw;
#else
    // Texture this polygon!
    float texw=1.0/(pnt->p3_z+Z_bias);
    texp->s=pnt->p3_u*texw;
    texp->t=pnt->p3_v*texw;
    texp->r=0;
    texp->w=texw;
#endif
    */
    texp->s = pnt->p3_u;
    texp->t = pnt->p3_v;
    texp->r = 0.0f;
    texp->w = 1.0f;

    // Finally, specify a vertex
    /*
    vertp->x=pnt->p3_sx+x_add;
    vertp->y=pnt->p3_sy+y_add;

    float z = max(0,min(1.0,1.0-(1.0/(pnt->p3_z+Z_bias))));
    vertp->z=-z;
    */
    *vertp = pnt->p3_vecPreRot;
  }

  // And draw!
  glDrawArrays(GL_POLYGON, 0, nv);
  OpenGL_polys_drawn++;
  OpenGL_verts_processed += nv;

  CHECK_ERROR(10)

  // If there is a lightmap to draw, draw it as well
  if (Overlay_type != OT_NONE) {
    return; // Temp fix until I figure out whats going on
    Int3(); // Shouldn't reach here
  }
}

// Takes nv vertices and draws the 2D polygon defined by those vertices.
// Uses bitmap "handle" as a texture
void rend_DrawPolygon2D(int handle, g3Point **p, int nv) {
  ASSERT(nv < 100);
  ASSERT(Overlay_type == OT_NONE);

  g3_RefreshTransforms(true);

  if (UseMultitexture) {
    opengl_SetMultitextureBlendMode(false);
  }

  int xAdd = OpenGL_state.clip_x1;
  int yAdd = OpenGL_state.clip_y1;

  float fr, fg, fb;
  if (OpenGL_state.cur_light_state == LS_FLAT_GOURAUD || OpenGL_state.cur_texture_quality == 0) {
    float scale = 1.0f / 255.0f;
    fr = GR_COLOR_RED(OpenGL_state.cur_color) * scale;
    fg = GR_COLOR_GREEN(OpenGL_state.cur_color) * scale;
    fb = GR_COLOR_BLUE(OpenGL_state.cur_color) * scale;
  }

  // make sure our bitmap is ready to be drawn
  opengl_MakeBitmapCurrent(handle, MAP_TYPE_BITMAP, 0);
  opengl_MakeWrapTypeCurrent(handle, MAP_TYPE_BITMAP, 0);

  float alpha = Alpha_multiplier * OpenGL_Alpha_factor;

  vector *vertp = &GL_verts[0];
  tex_array *texp = &GL_tex_coords[0];
  color_array *colorp = &GL_colors[0];

  // Specify our coordinates
  int i;
  for (i = 0; i < nv; ++i, ++vertp, ++texp, ++colorp) {
    g3Point *pnt = p[i];

    if (OpenGL_state.cur_alpha_type & ATF_VERTEX) {
      // the alpha should come from the vertex
      alpha = pnt->p3_a * Alpha_multiplier * OpenGL_Alpha_factor;
    }

    // If we have a lighting model, apply the correct lighting!
    if (OpenGL_state.cur_light_state == LS_FLAT_GOURAUD || OpenGL_state.cur_texture_quality == 0) {
      // pull the color from the constant color data
      colorp->r = fr;
      colorp->g = fg;
      colorp->b = fb;
      colorp->a = alpha;
    } else if (OpenGL_state.cur_light_state != LS_NONE) {
      // Do lighting based on intensity (MONO) or colored (RGB)
      if (OpenGL_state.cur_color_model == CM_MONO) {
        colorp->r = pnt->p3_l;
        colorp->g = pnt->p3_l;
        colorp->b = pnt->p3_l;
        colorp->a = alpha;
      } else {
        colorp->r = pnt->p3_r;
        colorp->g = pnt->p3_g;
        colorp->b = pnt->p3_b;
        colorp->a = alpha;
      }
    } else {
      // force white
      colorp->r = 1.0f;
      colorp->g = 1.0f;
      colorp->b = 1.0f;
      colorp->a = alpha;
    }

    texp->s = pnt->p3_u;
    texp->t = pnt->p3_v;
    texp->r = 0.0f;
    texp->w = 1.0f;

    // Finally, specify a vertex
    vertp->x = pnt->p3_sx + xAdd;
    vertp->y = pnt->p3_sy + yAdd;
    vertp->z = 0.0f;
  }

  // And draw!
  if (OpenGL_state.cur_texture_quality == 0) {
    // force disable textures
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  }

  // draw the data in the arrays
  glDrawArrays(GL_POLYGON, 0, nv);

  if (OpenGL_state.cur_texture_quality == 0) {
    // re-enable textures
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  }

  OpenGL_polys_drawn++;
  OpenGL_verts_processed += nv;

  CHECK_ERROR(10)
}


// Takes nv vertices and draws the 2D polygon defined by those vertices.
// Uses bitmap "handle" as a texture
void rend_DrawPolygon2D(d3Image *image, g3Point **p, int nv) {
  ASSERT(nv < 100);
  ASSERT(Overlay_type == OT_NONE);

  g3_RefreshTransforms(true);

  if (UseMultitexture) {
    opengl_SetMultitextureBlendMode(false);
  }

  int xAdd = OpenGL_state.clip_x1;
  int yAdd = OpenGL_state.clip_y1;

  float fr, fg, fb;
  if (OpenGL_state.cur_light_state == LS_FLAT_GOURAUD || OpenGL_state.cur_texture_quality == 0) {
    float scale = 1.0f / 255.0f;
    fr = GR_COLOR_RED(OpenGL_state.cur_color) * scale;
    fg = GR_COLOR_GREEN(OpenGL_state.cur_color) * scale;
    fb = GR_COLOR_BLUE(OpenGL_state.cur_color) * scale;
  }

  // make sure our bitmap is ready to be drawn
  // opengl_MakeBitmapCurrent(handle, MAP_TYPE_BITMAP, 0);
  // opengl_MakeWrapTypeCurrent(handle, MAP_TYPE_BITMAP, 0);
  image->Bind(0);

  float alpha = 1.0f;
   //Alpha_multiplier *OpenGL_Alpha_factor;

  vector *vertp = &GL_verts[0];
  tex_array *texp = &GL_tex_coords[0];
  color_array *colorp = &GL_colors[0];

  // Specify our coordinates
  int i;
  for (i = 0; i < nv; ++i, ++vertp, ++texp, ++colorp) {
    g3Point *pnt = p[i];

    if (OpenGL_state.cur_alpha_type & ATF_VERTEX) {
      // the alpha should come from the vertex
      alpha = pnt->p3_a;// * Alpha_multiplier * OpenGL_Alpha_factor;
    }

    // If we have a lighting model, apply the correct lighting!
    if (OpenGL_state.cur_light_state == LS_FLAT_GOURAUD || OpenGL_state.cur_texture_quality == 0) {
      // pull the color from the constant color data
      colorp->r = fr;
      colorp->g = fg;
      colorp->b = fb;
      colorp->a = alpha;
    } else if (OpenGL_state.cur_light_state != LS_NONE) {
      // Do lighting based on intensity (MONO) or colored (RGB)
      if (OpenGL_state.cur_color_model == CM_MONO) {
        colorp->r = pnt->p3_l;
        colorp->g = pnt->p3_l;
        colorp->b = pnt->p3_l;
        colorp->a = alpha;
      } else {
        colorp->r = pnt->p3_r;
        colorp->g = pnt->p3_g;
        colorp->b = pnt->p3_b;
        colorp->a = alpha;
      }
    } else {
      // force white
      colorp->r = 1.0f;
      colorp->g = 1.0f;
      colorp->b = 1.0f;
      colorp->a = alpha;
    }

    texp->s = pnt->p3_u;
    texp->t = pnt->p3_v;
    texp->r = 0.0f;
    texp->w = 1.0f;

    // Finally, specify a vertex
    vertp->x = pnt->p3_sx + xAdd;
    vertp->y = pnt->p3_sy + yAdd;
    vertp->z = 0.0f;
  }

  // And draw!
  if (OpenGL_state.cur_texture_quality == 0) {
    // force disable textures
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  }

  // draw the data in the arrays
  glDrawArrays(GL_POLYGON, 0, nv);

  if (OpenGL_state.cur_texture_quality == 0) {
    // re-enable textures
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  }

  OpenGL_polys_drawn++;
  OpenGL_verts_processed += nv;

  CHECK_ERROR(10)
}