#ifndef LR2_FOYERMESH_H
#define LR2_FOYERMESH_H

#include "types.h"

struct FoyerMeshAttachment;
void __fastcall UpdateFoyerMeshVertices(FoyerMeshAttachment*, void*);
void __fastcall UploadFoyerMeshVertices(FoyerMeshAttachment*, void*);
LegoBool32 TryUpdateFoyerMeshWithRenderer(FoyerMeshAttachment*);

#endif
