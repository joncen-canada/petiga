#include "petiga.h"

#undef  __FUNCT__
#define __FUNCT__ "IGAElementCreate"
PetscErrorCode IGAElementCreate(IGAElement *_element)
{
  IGAElement element;
  PetscErrorCode ierr;
  PetscFunctionBegin;
  PetscValidPointer(_element,1);
  ierr = PetscCalloc1(1,_element);CHKERRQ(ierr);
  element = *_element;
  element->refct =  1;
  element->index = -1;
  ierr = IGAPointCreate(&element->iterator);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGAElementDestroy"
PetscErrorCode IGAElementDestroy(IGAElement *_element)
{
  IGAElement     element;
  PetscErrorCode ierr;
  PetscFunctionBegin;
  PetscValidPointer(_element,1);
  element = *_element; *_element = NULL;
  if (!element) PetscFunctionReturn(0);
  if (--element->refct > 0) PetscFunctionReturn(0);
  ierr = IGAPointDestroy(&element->iterator);CHKERRQ(ierr);
  ierr = IGAElementReset(element);CHKERRQ(ierr);
  ierr = PetscFree(element);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGAElementReference"
PetscErrorCode IGAElementReference(IGAElement element)
{
  PetscFunctionBegin;
  PetscValidPointer(element,1);
  element->refct++;
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGAElementFreeWork"
static
PetscErrorCode IGAElementFreeWork(IGAElement element)
{
  PetscErrorCode ierr;
  PetscFunctionBegin;
  PetscValidPointer(element,1);
  {
    size_t MAX_WORK_VAL = sizeof(element->wval)/sizeof(PetscScalar*);
    size_t MAX_WORK_VEC = sizeof(element->wvec)/sizeof(PetscScalar*);
    size_t MAX_WORK_MAT = sizeof(element->wmat)/sizeof(PetscScalar*);
    size_t i;
    for (i=0; i<MAX_WORK_VAL; i++)
      {ierr = PetscFree(element->wval[i]);CHKERRQ(ierr);}
    element->nval = 0;
    for (i=0; i<MAX_WORK_VEC; i++)
      {ierr = PetscFree(element->wvec[i]);CHKERRQ(ierr);}
    element->nvec = 0;
    for (i=0; i<MAX_WORK_MAT; i++)
      {ierr = PetscFree(element->wmat[i]);CHKERRQ(ierr);}
    element->nmat = 0;
  }
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGAElementFreeFix"
static
PetscErrorCode IGAElementFreeFix(IGAElement element)
{
  PetscErrorCode ierr;
  PetscFunctionBegin;
  PetscValidPointer(element,1);
  element->nfix = 0;
  ierr = PetscFree(element->ifix);CHKERRQ(ierr);
  ierr = PetscFree(element->vfix);CHKERRQ(ierr);
  ierr = PetscFree(element->ufix);CHKERRQ(ierr);
  element->nflux = 0;
  ierr = PetscFree(element->iflux);CHKERRQ(ierr);
  ierr = PetscFree(element->vflux);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGAElementReset"
PetscErrorCode IGAElementReset(IGAElement element)
{
  PetscErrorCode ierr;
  PetscFunctionBegin;
  if (!element) PetscFunctionReturn(0);
  PetscValidPointer(element,1);
  element->count =  0;
  element->index = -1;

  if (element->rowmap != element->mapping)
    {ierr = PetscFree(element->rowmap);CHKERRQ(ierr);}
  if (element->colmap != element->mapping)
    {ierr = PetscFree(element->colmap);CHKERRQ(ierr);}
  element->rowmap = element->colmap = NULL;
  ierr = PetscFree(element->mapping);CHKERRQ(ierr);
  ierr = PetscFree(element->rationalW);CHKERRQ(ierr);
  ierr = PetscFree(element->geometryX);CHKERRQ(ierr);
  ierr = PetscFree(element->propertyA);CHKERRQ(ierr);

  ierr = PetscFree(element->point);CHKERRQ(ierr);
  ierr = PetscFree(element->weight);CHKERRQ(ierr);
  ierr = PetscFree(element->detJac);CHKERRQ(ierr);

  ierr = PetscFree(element->basis[0]);CHKERRQ(ierr);
  ierr = PetscFree(element->basis[1]);CHKERRQ(ierr);
  ierr = PetscFree(element->basis[2]);CHKERRQ(ierr);
  ierr = PetscFree(element->basis[3]);CHKERRQ(ierr);

  ierr = PetscFree(element->detX);CHKERRQ(ierr);
  ierr = PetscFree(element->gradX[0]);CHKERRQ(ierr);
  ierr = PetscFree(element->gradX[1]);CHKERRQ(ierr);
  ierr = PetscFree(element->hessX[0]);CHKERRQ(ierr);
  ierr = PetscFree(element->hessX[1]);CHKERRQ(ierr);
  ierr = PetscFree(element->der3X[0]);CHKERRQ(ierr);
  ierr = PetscFree(element->der3X[1]);CHKERRQ(ierr);
  ierr = PetscFree(element->detS);CHKERRQ(ierr);
  ierr = PetscFree(element->normal);CHKERRQ(ierr);

  ierr = PetscFree(element->shape[0]);CHKERRQ(ierr);
  ierr = PetscFree(element->shape[1]);CHKERRQ(ierr);
  ierr = PetscFree(element->shape[2]);CHKERRQ(ierr);
  ierr = PetscFree(element->shape[3]);CHKERRQ(ierr);

  ierr = IGAElementFreeFix(element);CHKERRQ(ierr);
  ierr = IGAElementFreeWork(element);CHKERRQ(ierr);

  ierr = IGAPointReset(element->iterator);CHKERRQ(ierr);

  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGAElementInit"
PetscErrorCode IGAElementInit(IGAElement element,IGA iga)
{
  PetscErrorCode ierr;
  PetscFunctionBegin;
  PetscValidPointer(element,1);
  PetscValidHeaderSpecific(iga,IGA_CLASSID,2);
  IGACheckSetUp(iga,1);
  ierr = IGAElementReset(element);CHKERRQ(ierr);
  element->parent = iga;
  element->collocation = iga->collocation;

  element->dof = iga->dof;
  element->dim = iga->dim;
  element->nsd = iga->geometry ? iga->geometry : iga->dim;
  element->npd = iga->property ? iga->property : 0;

  { /* */
    PetscInt *start = iga->elem_start;
    PetscInt *width = iga->elem_width;
    PetscInt *sizes = iga->elem_sizes;
    IGABasis *BD    = iga->basis;
    PetscInt i,dim = element->dim;
    PetscInt nel=1,nen=1,nqp=1;
    for (i=0; i<3; i++)
      element->ID[i] = 0;
    for (i=0; i<dim; i++) {
      element->start[i] = start[i];
      element->width[i] = width[i];
      element->sizes[i] = sizes[i];
      nel *= width[i];
      nen *= BD[i]->nen;
      nqp *= BD[i]->nqp;
    }
    for (i=dim; i<3; i++) {
      element->start[i] = 0;
      element->width[i] = 1;
      element->sizes[i] = 1;
    }
    element->index = -1;
    element->count = nel;
    element->nen   = nen;
    element->nqp   = nqp;
  }
  { /**/
    PetscInt nen = element->nen;
    PetscInt nsd = element->nsd;
    PetscInt npd = element->npd;
    ierr = PetscMalloc1(nen,&element->mapping);CHKERRQ(ierr);
    if (!element->collocation) {
      element->neq = nen;
      element->rowmap = element->mapping;
    } else {
      element->neq = 1;
      ierr = PetscMalloc1(1,&element->rowmap);CHKERRQ(ierr);
    }
    element->colmap = element->mapping;
    ierr = PetscMalloc1(nen,&element->rationalW);CHKERRQ(ierr);
    ierr = PetscMalloc1(nen*nsd,&element->geometryX);CHKERRQ(ierr);
    ierr = PetscMalloc1(nen*npd,&element->propertyA);CHKERRQ(ierr);
  }
  { /* */
    PetscInt nqp = element->nqp;
    PetscInt nen = element->nen;
    PetscInt dim = element->dim;

    ierr = PetscMalloc1(nqp*dim,&element->point);CHKERRQ(ierr);
    ierr = PetscMalloc1(nqp,&element->weight);CHKERRQ(ierr);
    ierr = PetscMalloc1(nqp,&element->detJac);CHKERRQ(ierr);

    ierr = PetscMalloc1(nqp*nen,&element->basis[0]);CHKERRQ(ierr);
    ierr = PetscMalloc1(nqp*nen*dim,&element->basis[1]);CHKERRQ(ierr);
    ierr = PetscMalloc1(nqp*nen*dim*dim,&element->basis[2]);CHKERRQ(ierr);
    ierr = PetscMalloc1(nqp*nen*dim*dim*dim,&element->basis[3]);CHKERRQ(ierr);

    ierr = PetscMalloc1(nqp,&element->detX);CHKERRQ(ierr);
    ierr = PetscMalloc1(nqp*dim*dim,&element->gradX[0]);CHKERRQ(ierr);
    ierr = PetscMalloc1(nqp*dim*dim,&element->gradX[1]);CHKERRQ(ierr);
    ierr = PetscMalloc1(nqp*dim*dim*dim,&element->hessX[0]);CHKERRQ(ierr);
    ierr = PetscMalloc1(nqp*dim*dim*dim,&element->hessX[1]);CHKERRQ(ierr);
    ierr = PetscMalloc1(nqp*dim*dim*dim*dim,&element->der3X[0]);CHKERRQ(ierr);
    ierr = PetscMalloc1(nqp*dim*dim*dim*dim,&element->der3X[1]);CHKERRQ(ierr);
    ierr = PetscMalloc1(nqp,&element->detS);CHKERRQ(ierr);
    ierr = PetscMalloc1(nqp*dim,&element->normal);CHKERRQ(ierr);

    ierr = PetscMalloc1(nqp*nen,&element->shape[0]);CHKERRQ(ierr);
    ierr = PetscMalloc1(nqp*nen*dim,&element->shape[1]);CHKERRQ(ierr);
    ierr = PetscMalloc1(nqp*nen*dim*dim,&element->shape[2]);CHKERRQ(ierr);
    ierr = PetscMalloc1(nqp*nen*dim*dim*dim,&element->shape[3]);CHKERRQ(ierr);
  }
  { /* */
    size_t MAX_WORK_VAL = sizeof(element->wval)/sizeof(PetscScalar*);
    size_t MAX_WORK_VEC = sizeof(element->wvec)/sizeof(PetscScalar*);
    size_t MAX_WORK_MAT = sizeof(element->wmat)/sizeof(PetscScalar*);
    size_t i, n = element->nen * element->dof;
    for (i=0; i<MAX_WORK_VAL; i++)
      {ierr = PetscMalloc1(n,&element->wval[i]);CHKERRQ(ierr);}
    for (i=0; i<MAX_WORK_VEC; i++)
      {ierr = PetscMalloc1(n,&element->wvec[i]);CHKERRQ(ierr);}
    for (i=0; i<MAX_WORK_MAT; i++)
      {ierr = PetscMalloc1(n*n,&element->wmat[i]);CHKERRQ(ierr);}
  }
  { /* */
    PetscInt nen = element->nen;
    PetscInt dof = element->dof;
    element->nfix = 0;
    ierr = PetscMalloc1(nen*dof,&element->ifix);CHKERRQ(ierr);
    ierr = PetscMalloc1(nen*dof,&element->vfix);CHKERRQ(ierr);
    ierr = PetscMalloc1(nen*dof,&element->ufix);CHKERRQ(ierr);
    element->nflux = 0;
    ierr = PetscMalloc1(nen*dof,&element->iflux);CHKERRQ(ierr);
    ierr = PetscMalloc1(nen*dof,&element->vflux);CHKERRQ(ierr);
  }
  ierr = IGAPointInit(element->iterator,element);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGAGetElement"
PetscErrorCode IGAGetElement(IGA iga,IGAElement *element)
{
  PetscFunctionBegin;
  PetscValidHeaderSpecific(iga,IGA_CLASSID,1);
  PetscValidPointer(element,2);
  *element = iga->iterator;
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGABeginElement"
PetscErrorCode IGABeginElement(IGA iga,IGAElement *_element)
{
  IGAElement     element;
  PetscErrorCode ierr;
  PetscFunctionBegin;
  PetscValidHeaderSpecific(iga,IGA_CLASSID,1);
  PetscValidPointer(_element,2);
  IGACheckSetUp(iga,1);
  element = *_element = iga->iterator;

  element->index = -1;
  element->atboundary  = PETSC_FALSE;
  element->boundary_id = -1;

  if (iga->rational && !iga->rationalW) SETERRQ(((PetscObject)iga)->comm,PETSC_ERR_ARG_WRONGSTATE,"No geometry set");
  if (iga->geometry && !iga->geometryX) SETERRQ(((PetscObject)iga)->comm,PETSC_ERR_ARG_WRONGSTATE,"No geometry set");
  if (iga->property && !iga->propertyA) SETERRQ(((PetscObject)iga)->comm,PETSC_ERR_ARG_WRONGSTATE,"No property set");
  element->rational = iga->rational ? PETSC_TRUE : PETSC_FALSE;
  element->geometry = iga->geometry ? PETSC_TRUE : PETSC_FALSE;
  element->property = iga->property ? PETSC_TRUE : PETSC_FALSE;
  { /* */
    PetscInt nen = element->nen;
    PetscInt nsd = iga->geometry ? iga->geometry : iga->dim;
    PetscInt npd = iga->property;
    if (element->nsd != nsd) {
      element->nsd = nsd;
      ierr = PetscFree(element->geometryX);CHKERRQ(ierr);
      ierr = PetscMalloc1(nen*nsd,&element->geometryX);CHKERRQ(ierr);
    }
    if (element->npd != npd) {
      element->npd = npd;
      ierr = PetscFree(element->propertyA);CHKERRQ(ierr);
      ierr = PetscMalloc1(nen*npd,&element->propertyA);CHKERRQ(ierr);
    }
    ierr = PetscMemzero(element->rationalW,sizeof(PetscReal)*nen);CHKERRQ(ierr);
    ierr = PetscMemzero(element->geometryX,sizeof(PetscReal)*nen*nsd);CHKERRQ(ierr);
    ierr = PetscMemzero(element->propertyA,sizeof(PetscReal)*nen*npd);CHKERRQ(ierr);
  }
  { /* */
    PetscInt q,i;
    PetscInt dim = element->dim;
    PetscInt nen = element->nen;
    PetscInt nqp = element->nqp;
    /* */
    ierr = PetscMemzero(element->point,   sizeof(PetscReal)*nqp*dim);CHKERRQ(ierr);
    ierr = PetscMemzero(element->weight,  sizeof(PetscReal)*nqp);CHKERRQ(ierr);
    ierr = PetscMemzero(element->detJac,  sizeof(PetscReal)*nqp);CHKERRQ(ierr);
    /* */
    ierr = PetscMemzero(element->basis[0],sizeof(PetscReal)*nqp*nen);CHKERRQ(ierr);
    ierr = PetscMemzero(element->basis[1],sizeof(PetscReal)*nqp*nen*dim);CHKERRQ(ierr);
    ierr = PetscMemzero(element->basis[2],sizeof(PetscReal)*nqp*nen*dim*dim);CHKERRQ(ierr);
    ierr = PetscMemzero(element->basis[3],sizeof(PetscReal)*nqp*nen*dim*dim*dim);CHKERRQ(ierr);
    /* */
    ierr = PetscMemzero(element->detX,    sizeof(PetscReal)*nqp);CHKERRQ(ierr);
    ierr = PetscMemzero(element->gradX[0],sizeof(PetscReal)*nqp*dim*dim);CHKERRQ(ierr);
    ierr = PetscMemzero(element->gradX[1],sizeof(PetscReal)*nqp*dim*dim);CHKERRQ(ierr);
    ierr = PetscMemzero(element->hessX[0],sizeof(PetscReal)*nqp*dim*dim*dim);CHKERRQ(ierr);
    ierr = PetscMemzero(element->hessX[1],sizeof(PetscReal)*nqp*dim*dim*dim);CHKERRQ(ierr);
    ierr = PetscMemzero(element->der3X[0],sizeof(PetscReal)*nqp*dim*dim*dim*dim);CHKERRQ(ierr);
    ierr = PetscMemzero(element->der3X[1],sizeof(PetscReal)*nqp*dim*dim*dim*dim);CHKERRQ(ierr);
    ierr = PetscMemzero(element->detS,    sizeof(PetscReal)*nqp);CHKERRQ(ierr);
    ierr = PetscMemzero(element->normal,  sizeof(PetscReal)*nqp*dim);CHKERRQ(ierr);
    /* */
    ierr = PetscMemzero(element->shape[0],sizeof(PetscReal)*nqp*nen);CHKERRQ(ierr);
    ierr = PetscMemzero(element->shape[1],sizeof(PetscReal)*nqp*nen*dim);CHKERRQ(ierr);
    ierr = PetscMemzero(element->shape[2],sizeof(PetscReal)*nqp*nen*dim*dim);CHKERRQ(ierr);
    ierr = PetscMemzero(element->shape[3],sizeof(PetscReal)*nqp*nen*dim*dim*dim);CHKERRQ(ierr);
    /* */
    for (q=0; q<nqp; q++) {
      PetscReal *G0 = &element->gradX[0][q*dim*dim];
      PetscReal *G1 = &element->gradX[1][q*dim*dim];
      element->detX[q] = 1.0;
      for (i=0; i<dim; i++)
        G0[i*(dim+1)] = G1[i*(dim+1)] = 1.0;
    }
  }
  { /* */
    IGAPoint point = element->iterator;
    point->neq = element->neq;
    point->nen = element->nen;
    point->dof = element->dof;
    point->dim = element->dim;
    point->nsd = element->nsd;
    point->npd = element->npd;
    point->rational = element->rational ? element->rationalW : NULL;
    point->geometry = element->geometry ? element->geometryX : NULL;
    point->property = element->property ? element->propertyA : NULL;
  }
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGANextElement"
PetscBool IGANextElement(IGA iga,IGAElement element)
{
  PetscInt i,dim  = element->dim;
  PetscInt *start = element->start;
  PetscInt *width = element->width;
  PetscInt *ID    = element->ID;
  PetscInt index,coord;
  PetscErrorCode ierr;
  /* */
  element->nval = 0;
  element->nvec = 0;
  element->nmat = 0;
  /* */
  index = ++element->index;
  if (PetscUnlikely(index >= element->count)) {
    element->index = -1;
    return PETSC_FALSE;
  }
  for (i=0; i<dim; i++) {
    coord = index % width[i];
    index = (index - coord) / width[i];
    ID[i] = coord + start[i];
  }
  /* */
#undef  CHKERRRETURN
#define CHKERRRETURN(n,r) do { if (PetscUnlikely(n)) { CHKERRCONTINUE(n); return (r);} } while (0)
  ierr = IGAElementBuildClosure(element);CHKERRRETURN(ierr,PETSC_FALSE);
  ierr = IGAElementBuildFix(element);CHKERRRETURN(ierr,PETSC_FALSE);
#undef  CHKERRRETURN
  return PETSC_TRUE;
}

#undef  __FUNCT__
#define __FUNCT__ "IGAEndElement"
PetscErrorCode IGAEndElement(IGA iga,IGAElement *element)
{
  PetscFunctionBegin;
  PetscValidHeaderSpecific(iga,IGA_CLASSID,1);
  PetscValidPointer(element,2);
  PetscValidPointer(*element,2);
  if (PetscUnlikely((*element)->index != -1)) {
    (*element)->index = -1;
    *element = NULL;
    PetscFunctionReturn(PETSC_ERR_PLIB);
  }
  *element = NULL;
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGAElementNextForm"
PetscBool IGAElementNextForm(IGAElement element,PetscBool visit[3][2])
{
  PetscInt dim = element->dim;
  while (++element->boundary_id < 2*dim) {
    PetscInt i = element->boundary_id / 2;
    PetscInt s = element->boundary_id % 2;
    PetscInt e = s ? element->sizes[i]-1 : 0;
    if (element->ID[i] != e) continue;
    if (!visit[i][s]) continue;
    element->atboundary = PETSC_TRUE;
    return PETSC_TRUE;
  }
  if (element->boundary_id++ == 2*dim) {
    element->atboundary = PETSC_FALSE;
    return PETSC_TRUE;
  }
  element->atboundary  = PETSC_FALSE;
  element->boundary_id = -1;
  return PETSC_FALSE;
}

#undef  __FUNCT__
#define __FUNCT__ "IGAElementGetPoint"
PetscErrorCode IGAElementGetPoint(IGAElement element,IGAPoint *point)
{
  PetscFunctionBegin;
  PetscValidPointer(element,1);
  PetscValidPointer(point,2);
  *point = element->iterator;
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGAElementBeginPoint"
PetscErrorCode IGAElementBeginPoint(IGAElement element,IGAPoint *_point)
{
  IGAPoint       point;
  PetscErrorCode ierr;
  PetscFunctionBegin;
  PetscValidPointer(element,1);
  PetscValidPointer(_point,2);
  if (PetscUnlikely(element->index < 0))
    SETERRQ(PETSC_COMM_SELF,PETSC_ERR_ARG_WRONGSTATE,"Must call during element loop");
  point = *_point = element->iterator;

  point->index = -1;
  point->count = element->nqp;

  point->atboundary  = element->atboundary;
  point->boundary_id = element->boundary_id;

  if (PetscLikely(!element->atboundary)) {
    ierr = IGAElementBuildQuadrature(element);CHKERRQ(ierr);
    ierr = IGAElementBuildShapeFuns(element);CHKERRQ(ierr);
  } else {
    PetscInt axis = element->boundary_id / 2;
    PetscInt side = element->boundary_id % 2;
    point->count = element->nqp / element->parent->basis[axis]->nqp;
    ierr = IGAElementBuildQuadratureAtBoundary(element,axis,side);CHKERRQ(ierr);
    ierr = IGAElementBuildShapeFunsAtBoundary (element,axis,side);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGAElementNextPoint"
PetscBool IGAElementNextPoint(IGAElement element,IGAPoint point)
{
  PetscInt nen = point->nen;
  PetscInt dim = point->dim;
  PetscInt dim2 = dim*dim;
  PetscInt dim3 = dim*dim2;
  PetscInt dim4 = dim*dim3;
  PetscInt index;
  /* */
  point->nvec = 0;
  point->nmat = 0;
  /* */
  index = ++point->index;
  if (PetscUnlikely(index == 0))            goto start;
  if (PetscUnlikely(index >= point->count)) goto stop;

  point->point    += dim;
  point->weight   += 1;
  point->detJac   += 1;

  point->basis[0] += nen;
  point->basis[1] += nen*dim;
  point->basis[2] += nen*dim2;
  point->basis[3] += nen*dim3;

  point->detX     += 1;
  point->gradX[0] += dim2;
  point->gradX[1] += dim2;
  point->hessX[0] += dim3;
  point->hessX[1] += dim3;
  point->der3X[0] += dim4;
  point->der3X[1] += dim4;
  point->detS     += 1;
  point->normal   += dim;

  point->shape[0] += nen;
  point->shape[1] += nen*dim;
  point->shape[2] += nen*dim2;
  point->shape[3] += nen*dim3;

  return PETSC_TRUE;

 start:

  point->point    = element->point;
  point->weight   = element->weight;
  point->detJac   = element->detJac;

  point->basis[0] = element->basis[0];
  point->basis[1] = element->basis[1];
  point->basis[2] = element->basis[2];
  point->basis[3] = element->basis[3];

  point->detX     = element->detX;
  point->gradX[0] = element->gradX[0];
  point->gradX[1] = element->gradX[1];
  point->hessX[0] = element->hessX[0];
  point->hessX[1] = element->hessX[1];
  point->der3X[0] = element->der3X[0];
  point->der3X[1] = element->der3X[1];
  point->detS     = element->detS;
  point->normal   = element->normal;

  if (element->geometry && dim == point->nsd) { /* XXX */
    point->shape[0] = element->shape[0];
    point->shape[1] = element->shape[1];
    point->shape[2] = element->shape[2];
    point->shape[3] = element->shape[3];
  } else {
    point->shape[0] = element->basis[0];
    point->shape[1] = element->basis[1];
    point->shape[2] = element->basis[2];
    point->shape[3] = element->basis[3];
  }

  return PETSC_TRUE;

 stop:

  point->index = -1;
  return PETSC_FALSE;
}

#undef  __FUNCT__
#define __FUNCT__ "IGAElementEndPoint"
PetscErrorCode IGAElementEndPoint(IGAElement element,IGAPoint *point)
{
  PetscErrorCode ierr;
  PetscFunctionBegin;
  PetscValidPointer(element,1);
  PetscValidPointer(point,2);
  PetscValidPointer(*point,2);
  if (PetscUnlikely((*point)->index != -1)) {
    (*point)->index = -1;
    PetscFunctionReturn(PETSC_ERR_PLIB);
  }
  *point = NULL;
  /* XXX */
  if (PetscUnlikely(element->atboundary)) {
    PetscInt  nqp = element->nqp,  dim = element->dim;
    PetscReal *S  = element->detS, *n  = element->normal;
    ierr = PetscMemzero(S,nqp*sizeof(PetscReal));CHKERRQ(ierr);
    ierr = PetscMemzero(n,nqp*dim*sizeof(PetscReal));CHKERRQ(ierr);
    if (PetscUnlikely(element->collocation)) {
      element->atboundary  = PETSC_FALSE;
      element->boundary_id = 2*element->dim;
    }
  }
  /* XXX */
 PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGAElementGetParent"
PetscErrorCode IGAElementGetParent(IGAElement element,IGA *parent)
{
  PetscFunctionBegin;
  PetscValidPointer(element,1);
  PetscValidIntPointer(parent,2);
  *parent = element->parent;
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGAElementGetIndex"
PetscErrorCode IGAElementGetIndex(IGAElement element,PetscInt *index)
{
  PetscFunctionBegin;
  PetscValidPointer(element,1);
  PetscValidIntPointer(index,2);
  *index = element->index;
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGAElementGetCount"
PetscErrorCode IGAElementGetCount(IGAElement element,PetscInt *count)
{
  PetscFunctionBegin;
  PetscValidPointer(element,1);
  PetscValidIntPointer(count,2);
  *count = element->count;
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGAElementGetSizes"
PetscErrorCode IGAElementGetSizes(IGAElement element,PetscInt *neq,PetscInt *nen,PetscInt *dof)
{
  PetscFunctionBegin;
  PetscValidPointer(element,1);
  if (neq) PetscValidIntPointer(neq,2);
  if (nen) PetscValidIntPointer(nen,3);
  if (dof) PetscValidIntPointer(dof,4);
  if (neq) *neq = element->neq;
  if (nen) *nen = element->nen;
  if (dof) *dof = element->dof;
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGAElementGetClosure"
PetscErrorCode IGAElementGetClosure(IGAElement element,PetscInt *nen,const PetscInt *mapping[])
{
  PetscFunctionBegin;
  PetscValidPointer(element,1);
  if (nen)     PetscValidIntPointer(nen,2);
  if (mapping) PetscValidPointer(mapping,3);
  if (nen)     *nen     = element->nen;
  if (mapping) *mapping = element->mapping;
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGAElementGetIndices"
PetscErrorCode IGAElementGeIndices(IGAElement element,
                                   PetscInt *neq,const PetscInt *rowmap[],
                                   PetscInt *nen,const PetscInt *colmap[])
{
  PetscFunctionBegin;
  PetscValidPointer(element,1);
  if (neq)    PetscValidIntPointer(neq,2);
  if (rowmap) PetscValidPointer(rowmap,3);
  if (nen)    PetscValidIntPointer(nen,4);
  if (colmap) PetscValidPointer(colmap,5);
  if (neq)    *neq    = element->neq;
  if (colmap) *colmap = element->rowmap;
  if (nen)    *nen    = element->nen;
  if (colmap) *colmap = element->colmap;
  PetscFunctionReturn(0);
}

/* -------------------------------------------------------------------------- */

#undef  __FUNCT__
#define __FUNCT__ "IGAElementBuildClosure"
PetscErrorCode IGAElementBuildClosure(IGAElement element)
{
  PetscFunctionBegin;
  PetscValidPointer(element,1);
  if (PetscUnlikely(element->index < 0))
    SETERRQ(PETSC_COMM_SELF,PETSC_ERR_ARG_WRONGSTATE,"Must call during element loop");
  { /* */
    IGA      iga = element->parent;
    IGABasis *BD = element->parent->basis;
    PetscInt *ID = element->ID;
    PetscInt ia, inen = BD[0]->nen, ioffset = BD[0]->offset[ID[0]];
    PetscInt ja, jnen = BD[1]->nen, joffset = BD[1]->offset[ID[1]];
    PetscInt ka, knen = BD[2]->nen, koffset = BD[2]->offset[ID[2]];
    PetscInt *start = iga->node_gstart, *width = iga->node_gwidth;
    PetscInt istart = start[0]/*istride = 1*/;
    PetscInt jstart = start[1], jstride = width[0];
    PetscInt kstart = start[2], kstride = width[0]*width[1];
    PetscInt a=0, *mapping = element->mapping;
    for (ka=0; ka<knen; ka++) {
      for (ja=0; ja<jnen; ja++) {
        for (ia=0; ia<inen; ia++) {
          PetscInt iA = (ioffset + ia) - istart;
          PetscInt jA = (joffset + ja) - jstart;
          PetscInt kA = (koffset + ka) - kstart;
          mapping[a++] = iA + jA*jstride + kA*kstride;
        }
      }
    }
    if (PetscUnlikely(element->collocation)) {
      PetscInt iA = ID[0] - istart;
      PetscInt jA = ID[1] - jstart;
      PetscInt kA = ID[2] - kstart;
      element->rowmap[0] = iA + jA*jstride + kA*kstride;
    }
  }
  { /* */
    PetscInt a,nen = element->nen;
    PetscInt *map = element->mapping;
    if (element->rational) {
      PetscReal *arrayW = element->parent->rationalW;
      PetscReal *W = element->rationalW;
      for (a=0; a<nen; a++)
        W[a] = arrayW[map[a]];
    }
    if (element->geometry) {
      PetscReal *arrayX = element->parent->geometryX;
      PetscReal *X = element->geometryX;
      PetscInt  i,nsd = element->nsd;
      for (a=0; a<nen; a++)
        for (i=0; i<nsd; i++)
          X[i + a*nsd] = arrayX[map[a]*nsd + i];
    }
    if (element->property) {
      PetscScalar *arrayA = element->parent->propertyA;
      PetscScalar *A = element->propertyA;
      PetscInt    i,npd = element->npd;
      for (a=0; a<nen; a++)
        for (i=0; i<npd; i++)
          A[i + a*npd] = arrayA[map[a]*npd + i];
    }
  }
  PetscFunctionReturn(0);
}
/* -------------------------------------------------------------------------- */

EXTERN_C_BEGIN
extern void IGA_Quadrature_1D(PetscInt,const PetscReal[],const PetscReal[],const PetscReal*,
                              PetscReal[],PetscReal[],PetscReal[]);
extern void IGA_Quadrature_2D(PetscInt,const PetscReal[],const PetscReal[],const PetscReal*,
                              PetscInt,const PetscReal[],const PetscReal[],const PetscReal*,
                              PetscReal[],PetscReal[],PetscReal[]);
extern void IGA_Quadrature_3D(PetscInt,const PetscReal[],const PetscReal[],const PetscReal*,
                              PetscInt,const PetscReal[],const PetscReal[],const PetscReal*,
                              PetscInt,const PetscReal[],const PetscReal[],const PetscReal*,
                              PetscReal[],PetscReal[],PetscReal[]);
EXTERN_C_END

EXTERN_C_BEGIN
extern void IGA_BasisFuns_1D(PetscInt,PetscInt,const PetscReal[],
                             PetscInt,PetscInt,PetscInt,const PetscReal[],
                             PetscReal[],PetscReal[],PetscReal[],PetscReal[]);
extern void IGA_BasisFuns_2D(PetscInt,PetscInt,const PetscReal[],
                             PetscInt,PetscInt,PetscInt,const PetscReal[],
                             PetscInt,PetscInt,PetscInt,const PetscReal[],
                             PetscReal[],PetscReal[],PetscReal[],PetscReal[]);
extern void IGA_BasisFuns_3D(PetscInt,PetscInt,const PetscReal[],
                             PetscInt,PetscInt,PetscInt,const PetscReal[],
                             PetscInt,PetscInt,PetscInt,const PetscReal[],
                             PetscInt,PetscInt,PetscInt,const PetscReal[],
                             PetscReal[],PetscReal[],PetscReal[],PetscReal[]);
EXTERN_C_END

EXTERN_C_BEGIN
extern void IGA_ShapeFuns_1D(PetscInt,PetscInt,PetscInt,const PetscReal[],
                             const PetscReal[],const PetscReal[],const PetscReal[],const PetscReal[],
                             PetscReal[],PetscReal[],PetscReal[],PetscReal[],
                             PetscReal[],PetscReal[],PetscReal[],PetscReal[],PetscReal[],PetscReal[],PetscReal[]);
extern void IGA_ShapeFuns_2D(PetscInt,PetscInt,PetscInt,const PetscReal[],
                             const PetscReal[],const PetscReal[],const PetscReal[],const PetscReal[],
                             PetscReal[],PetscReal[],PetscReal[],PetscReal[],
                             PetscReal[],PetscReal[],PetscReal[],PetscReal[],PetscReal[],PetscReal[],PetscReal[]);
extern void IGA_ShapeFuns_3D(PetscInt,PetscInt,PetscInt,const PetscReal[],
                             const PetscReal[],const PetscReal[],const PetscReal[],const PetscReal[],
                             PetscReal[],PetscReal[],PetscReal[],PetscReal[],
                             PetscReal[],PetscReal[],PetscReal[],PetscReal[],PetscReal[],PetscReal[],PetscReal[]);
EXTERN_C_END

#define IGA_Quadrature_ARGS(ID,BD,i) \
  BD[i]->nqp,BD[i]->point+ID[i]*BD[i]->nqp,BD[i]->weight,BD[i]->detJ+ID[i]

#define IGA_BasisFuns_ARGS(ID,BD,i) \
  BD[i]->nqp,BD[i]->nen,BD[i]->d,BD[i]->value+ID[i]*BD[i]->nqp*BD[i]->nen*(BD[i]->d+1)

#undef  __FUNCT__
#define __FUNCT__ "IGAElementBuildQuadrature"
PetscErrorCode IGAElementBuildQuadrature(IGAElement element)
{
  PetscFunctionBegin;
  PetscValidPointer(element,1);
  if (PetscUnlikely(element->index < 0))
    SETERRQ(PETSC_COMM_SELF,PETSC_ERR_ARG_WRONGSTATE,"Must call during element loop");
  {
    IGABasis *BD = element->parent->basis;
    PetscInt *ID = element->ID;
    PetscReal *u = element->point;
    PetscReal *w = element->weight;
    PetscReal *J = element->detJac;
    switch (element->dim) {
    case 3: IGA_Quadrature_3D(IGA_Quadrature_ARGS(ID,BD,0),
                              IGA_Quadrature_ARGS(ID,BD,1),
                              IGA_Quadrature_ARGS(ID,BD,2),
                              u,w,J); break;
    case 2: IGA_Quadrature_2D(IGA_Quadrature_ARGS(ID,BD,0),
                              IGA_Quadrature_ARGS(ID,BD,1),
                              u,w,J); break;
    case 1: IGA_Quadrature_1D(IGA_Quadrature_ARGS(ID,BD,0),
                              u,w,J); break;
    }
  }
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGAElementBuildShapeFuns"
PetscErrorCode IGAElementBuildShapeFuns(IGAElement element)
{
  PetscFunctionBegin;
  PetscValidPointer(element,1);
  if (PetscUnlikely(element->index < 0))
    SETERRQ(PETSC_COMM_SELF,PETSC_ERR_ARG_WRONGSTATE,"Must call during element loop");
  {
    IGABasis  *BD = element->parent->basis;
    PetscInt  *ID = element->ID;
    PetscInt  ord = element->parent->order;
    PetscInt  rat = element->rational;
    PetscReal *W  = element->rationalW;
    PetscReal **N = element->basis;
    switch (element->dim) {
    case 3: IGA_BasisFuns_3D(ord,rat,W,
                             IGA_BasisFuns_ARGS(ID,BD,0),
                             IGA_BasisFuns_ARGS(ID,BD,1),
                             IGA_BasisFuns_ARGS(ID,BD,2),
                             N[0],N[1],N[2],N[3]); break;
    case 2: IGA_BasisFuns_2D(ord,rat,W,
                             IGA_BasisFuns_ARGS(ID,BD,0),
                             IGA_BasisFuns_ARGS(ID,BD,1),
                             N[0],N[1],N[2],N[3]); break;
    case 1: IGA_BasisFuns_1D(ord,rat,W,
                             IGA_BasisFuns_ARGS(ID,BD,0),
                             N[0],N[1],N[2],N[3]); break;
    }
  }

  if (element->dim == element->nsd) /* XXX */
  if (element->geometry) {
    PetscInt q;
    PetscInt ord  = element->parent->order;
    PetscInt nqp  = element->nqp;
    PetscInt nen  = element->nen;
    PetscReal *X  = element->geometryX;
    PetscReal **M = element->basis;
    PetscReal **N = element->shape;
    PetscReal *J  = element->detX;
    PetscReal *G0 = element->gradX[0];
    PetscReal *G1 = element->gradX[1];
    PetscReal *H0 = element->hessX[0];
    PetscReal *H1 = element->hessX[1];
    PetscReal *I0 = element->der3X[0];
    PetscReal *I1 = element->der3X[1];
    switch (element->dim) {
    case 3: IGA_ShapeFuns_3D(ord,nqp,nen,X,
                             M[0],M[1],M[2],M[3],
                             N[0],N[1],N[2],N[3],
                             J,G0,G1,H0,H1,I0,I1); break;
    case 2: IGA_ShapeFuns_2D(ord,nqp,nen,X,
                             M[0],M[1],M[2],M[3],
                             N[0],N[1],N[2],N[3],
                             J,G0,G1,H0,H1,I0,I1); break;
    case 1: IGA_ShapeFuns_1D(ord,nqp,nen,X,
                             M[0],M[1],M[2],M[3],
                             N[0],N[1],N[2],N[3],
                             J,G0,G1,H0,H1,I0,I1); break;
    }
    for (q=0; q<nqp; q++)
      element->detJac[q] *= J[q];
  }
  PetscFunctionReturn(0);
}

#define IGA_Quadrature_BNDR(ID,BD,i,s) \
  1,&BD[i]->bnd_point[s],&BD[i]->bnd_weight[s],&BD[i]->bnd_detJ[s]

#define IGA_BasisFuns_BNDR(ID,BD,i,s) \
  1,BD[i]->nen,BD[i]->d,BD[i]->bnd_value[s]

EXTERN_C_BEGIN
extern void IGA_GetNormal(PetscInt dim,PetscInt axis,PetscInt side,const PetscReal F[],PetscReal *dS,PetscReal N[]);
EXTERN_C_END

#undef  __FUNCT__
#define __FUNCT__ "IGAElementBuildQuadratureAtBoundary"
PetscErrorCode IGAElementBuildQuadratureAtBoundary(IGAElement element,PetscInt axis,PetscInt side)
{
  PetscFunctionBegin;
  PetscValidPointer(element,1);
  if (PetscUnlikely(element->index < 0))
    SETERRQ(PETSC_COMM_SELF,PETSC_ERR_ARG_WRONGSTATE,"Must call during element loop");
  {
    IGABasis *BD = element->parent->basis;
    PetscInt *ID = element->ID;
    PetscReal *u = element->point;
    PetscReal *w = element->weight;
    PetscReal *J = element->detJac;
    switch (element->dim) {
    case 3:
      switch (axis) {
      case 0: IGA_Quadrature_3D(IGA_Quadrature_BNDR(ID,BD,0,side),
                                IGA_Quadrature_ARGS(ID,BD,1),
                                IGA_Quadrature_ARGS(ID,BD,2),
                                u,w,J); break;
      case 1: IGA_Quadrature_3D(IGA_Quadrature_ARGS(ID,BD,0),
                                IGA_Quadrature_BNDR(ID,BD,1,side),
                                IGA_Quadrature_ARGS(ID,BD,2),
                                u,w,J); break;
      case 2: IGA_Quadrature_3D(IGA_Quadrature_ARGS(ID,BD,0),
                                IGA_Quadrature_ARGS(ID,BD,1),
                                IGA_Quadrature_BNDR(ID,BD,2,side),
                                u,w,J); break;
      } break;
    case 2:
      switch (axis) {
      case 0: IGA_Quadrature_2D(IGA_Quadrature_BNDR(ID,BD,0,side),
                                IGA_Quadrature_ARGS(ID,BD,1),
                                u,w,J); break;
      case 1: IGA_Quadrature_2D(IGA_Quadrature_ARGS(ID,BD,0),
                                IGA_Quadrature_BNDR(ID,BD,1,side),
                                u,w,J); break;
      } break;
    case 1:
      switch (axis) {
      case 0: IGA_Quadrature_1D(IGA_Quadrature_BNDR(ID,BD,0,side),
                                u,w,J); break;
      } break;
    }
  }
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGAElementBuildShapeFunsAtBoundary"
PetscErrorCode IGAElementBuildShapeFunsAtBoundary(IGAElement element,PetscInt axis,PetscInt side)
{
  PetscFunctionBegin;
  PetscValidPointer(element,1);
  if (PetscUnlikely(element->index < 0))
    SETERRQ(PETSC_COMM_SELF,PETSC_ERR_ARG_WRONGSTATE,"Must call during element loop");
  {
    IGABasis  *BD = element->parent->basis;
    PetscInt  *ID = element->ID;
    PetscInt  ord = element->parent->order;
    PetscInt  rat = element->rational;
    PetscReal *W  = element->rationalW;
    PetscReal **N = element->basis;
    switch (element->dim) {
    case 3:
      switch (axis) {
      case 0: IGA_BasisFuns_3D(ord,rat,W,
                               IGA_BasisFuns_BNDR(ID,BD,0,side),
                               IGA_BasisFuns_ARGS(ID,BD,1),
                               IGA_BasisFuns_ARGS(ID,BD,2),
                               N[0],N[1],N[2],N[3]); break;
      case 1: IGA_BasisFuns_3D(ord,rat,W,
                               IGA_BasisFuns_ARGS(ID,BD,0),
                               IGA_BasisFuns_BNDR(ID,BD,1,side),
                               IGA_BasisFuns_ARGS(ID,BD,2),
                               N[0],N[1],N[2],N[3]); break;
      case 2: IGA_BasisFuns_3D(ord,rat,W,
                               IGA_BasisFuns_ARGS(ID,BD,0),
                               IGA_BasisFuns_ARGS(ID,BD,1),
                               IGA_BasisFuns_BNDR(ID,BD,2,side),
                               N[0],N[1],N[2],N[3]); break;
      } break;
    case 2:
      switch (axis) {
      case 0: IGA_BasisFuns_2D(ord,rat,W,
                               IGA_BasisFuns_BNDR(ID,BD,0,side),
                               IGA_BasisFuns_ARGS(ID,BD,1),
                               N[0],N[1],N[2],N[3]); break;
      case 1: IGA_BasisFuns_2D(ord,rat,W,
                               IGA_BasisFuns_ARGS(ID,BD,0),
                               IGA_BasisFuns_BNDR(ID,BD,1,side),
                               N[0],N[1],N[2],N[3]); break;
      } break;
    case 1:
      switch (axis) {
      case 0: IGA_BasisFuns_1D(ord,rat,W,
                               IGA_BasisFuns_BNDR(ID,BD,0,side),
                               N[0],N[1],N[2],N[3]); break;
      } break;
    }
    {
      PetscInt q;
      PetscInt nqp = element->nqp / element->parent->basis[axis]->nqp;
      PetscInt dim = element->dim;
      PetscReal *S = element->detS;
      PetscReal *n = element->normal;
      (void)PetscMemzero(n,nqp*dim*sizeof(PetscReal));
      for (q=0; q<nqp; q++) S[q] = 1.0;
      for (q=0; q<nqp; q++) n[q*dim+axis] = side ? 1.0 : -1.0;
    }
  }

  if (element->dim == element->nsd) /* XXX */
  if (element->geometry) {
    PetscInt q;
    PetscInt dim  = element->dim;
    PetscInt ord  = element->parent->order;
    PetscInt nqp  = element->nqp / element->parent->basis[axis]->nqp;
    PetscInt nen  = element->nen;
    PetscReal *X  = element->geometryX;
    PetscReal **M = element->basis;
    PetscReal **N = element->shape;
    PetscReal *J  = element->detX;
    PetscReal *G0 = element->gradX[0];
    PetscReal *G1 = element->gradX[1];
    PetscReal *H0 = element->hessX[0];
    PetscReal *H1 = element->hessX[1];
    PetscReal *I0 = element->der3X[0];
    PetscReal *I1 = element->der3X[1];
    PetscReal *S  = element->detS;
    PetscReal *n  = element->normal;
    switch (dim) {
    case 3: IGA_ShapeFuns_3D(ord,nqp,nen,X,
                             M[0],M[1],M[2],M[3],
                             N[0],N[1],N[2],N[3],
                             J,G0,G1,H0,H1,I0,I1); break;
    case 2: IGA_ShapeFuns_2D(ord,nqp,nen,X,
                             M[0],M[1],M[2],M[3],
                             N[0],N[1],N[2],N[3],
                             J,G0,G1,H0,H1,I0,I1); break;
    case 1: IGA_ShapeFuns_1D(ord,nqp,nen,X,
                             M[0],M[1],M[2],M[3],
                             N[0],N[1],N[2],N[3],
                             J,G0,G1,H0,H1,I0,I1); break;
    }
    for (q=0; q<nqp; q++)
      IGA_GetNormal(dim,axis,side,&G0[q*dim*dim],&S[q],&n[q*dim]);
    for (q=0; q<nqp; q++)
      element->detJac[q] *= S[q];
  }
  PetscFunctionReturn(0);
}

/* -------------------------------------------------------------------------- */

#undef  __FUNCT__
#define __FUNCT__ "IGAElementGetWorkVec"
PetscErrorCode IGAElementGetWorkVec(IGAElement element,PetscScalar *V[])
{
  PetscErrorCode ierr;
  PetscFunctionBegin;
  PetscValidPointer(element,1);
  PetscValidPointer(V,2);
  if (PetscUnlikely(element->index < 0))
    SETERRQ(PETSC_COMM_SELF,PETSC_ERR_ARG_WRONGSTATE,"Must call during element loop");
  if (PetscUnlikely(element->nvec >= sizeof(element->wvec)/sizeof(PetscScalar*)))
    SETERRQ(PETSC_COMM_SELF,PETSC_ERR_ARG_OUTOFRANGE,"Too many work vectors requested");
  {
    size_t m = (size_t)(element->neq * element->dof);
    *V = element->wvec[element->nvec++];
    ierr = PetscMemzero(*V,m*sizeof(PetscScalar));CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGAElementGetWorkMat"
PetscErrorCode IGAElementGetWorkMat(IGAElement element,PetscScalar *M[])
{
  PetscErrorCode ierr;
  PetscFunctionBegin;
  PetscValidPointer(element,1);
  PetscValidPointer(M,2);
  if (PetscUnlikely(element->index < 0))
    SETERRQ(PETSC_COMM_SELF,PETSC_ERR_ARG_WRONGSTATE,"Must call during element loop");
  if (PetscUnlikely(element->nmat >= sizeof(element->wmat)/sizeof(PetscScalar*)))
    SETERRQ(PETSC_COMM_SELF,PETSC_ERR_ARG_OUTOFRANGE,"Too many work matrices requested");
  {
    size_t m = (size_t)(element->neq * element->dof);
    size_t n = (size_t)(element->nen * element->dof);
    *M = element->wmat[element->nmat++];
    ierr = PetscMemzero(*M,m*n*sizeof(PetscScalar));CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGAElementGetValues"
PetscErrorCode IGAElementGetValues(IGAElement element,const PetscScalar arrayU[],PetscScalar *_U[])
{
  PetscScalar    *U;
  PetscErrorCode ierr;
  PetscFunctionBegin;
  PetscValidPointer(element,1);
  if (arrayU) PetscValidScalarPointer(arrayU,2);
  PetscValidScalarPointer(_U,3);
  if (PetscUnlikely(element->index < 0))
    SETERRQ(PETSC_COMM_SELF,PETSC_ERR_ARG_WRONGSTATE,"Must call during element loop");
  if (PetscUnlikely((size_t)element->nval >= sizeof(element->wval)/sizeof(PetscScalar*)))
    SETERRQ(PETSC_COMM_SELF,PETSC_ERR_ARG_OUTOFRANGE,"Too many work values requested");
  U = *_U = element->wval[element->nval++];
  if (PetscLikely(arrayU)) {
    PetscInt a, nen = element->nen;
    PetscInt i, dof = element->dof;
    PetscInt pos = 0, *map = element->mapping;
    for (a=0; a<nen; a++) {
      const PetscScalar *u = arrayU + map[a]*dof;
      for (i=0; i<dof; i++)
        U[pos++] = u[i]; /* XXX Use PetscMemcpy() ?? */
    }
  } else {
    size_t n = (size_t)(element->nen * element->dof);
    ierr = PetscMemzero(U,n*sizeof(PetscScalar));CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

/* -------------------------------------------------------------------------- */

EXTERN_C_BEGIN
extern void IGA_BoundaryArea_2D(const PetscInt[],PetscInt,PetscInt,
                                PetscInt,const PetscReal[],
                                PetscInt,const PetscReal[],
                                PetscInt,const PetscReal[],PetscInt,PetscInt,const PetscReal[],
                                PetscReal*);
extern void IGA_BoundaryArea_3D(const PetscInt[],PetscInt,PetscInt,
                                PetscInt,const PetscReal[],
                                PetscInt,const PetscReal[],
                                PetscInt,const PetscReal[],PetscInt,PetscInt,const PetscReal[],
                                PetscInt,const PetscReal[],PetscInt,PetscInt,const PetscReal[],
                                PetscReal*);
EXTERN_C_END

static PetscReal BoundaryArea(IGAElement element,PetscInt dir,PetscInt side)
{
  PetscReal A = 1.0;
  PetscInt *ID = element->ID;
  IGABasis *BD = element->parent->basis;
  PetscInt i,dim = element->dim;
  if (dim == 1) return A;
  for (i=0; i<dim; i++)
    if (i != dir) {
      PetscReal L = BD[i]->detJ[ID[i]];
      PetscInt  n = BD[i]->nen;
      A *= L/n;
    }
  if (!element->geometry) {
    A *= (dim==2) ? 2 : 4; /* sum(W) = 2 */
  } else {
    PetscInt shape[3] = {1,1,1};
    PetscInt k,nqp[3],nen[3],ndr[3];
    PetscReal *W[3],*N[3],dS = 1.0;
    for (i=0; i<dim; i++)
      shape[i] = BD[i]->nen;
    for (k=0,i=0; i<dim; i++) {
      if (i == dir) continue;
      nqp[k] = BD[i]->nqp;
      nen[k] = BD[i]->nen;
      ndr[k] = BD[i]->d;
      W[k]   = BD[i]->weight;
      N[k]   = BD[i]->value+ID[i]*nqp[k]*nen[k]*(ndr[k]+1);
      k++;
    }
    switch (dim) {
    case 2: IGA_BoundaryArea_2D(shape,dir,side,
                                element->geometry,element->geometryX,
                                element->rational,element->rationalW,
                                nqp[0],W[0],nen[0],ndr[0],N[0],
                                &dS); break;
    case 3: IGA_BoundaryArea_3D(shape,dir,side,
                                element->geometry,element->geometryX,
                                element->rational,element->rationalW,
                                nqp[0],W[0],nen[0],ndr[0],N[0],
                                nqp[1],W[1],nen[1],ndr[1],N[1],
                                &dS);break;
    }
    A *= dS;
  }
  return A;
}

static void AddFixa(IGAElement element,IGAFormBC bc,PetscInt a)
{
  if (bc->count) {
    IGA iga = element->parent;
    PetscInt dof = element->dof;
    PetscInt count = element->nfix;
    PetscInt *index = element->ifix;
    PetscScalar *value = element->vfix;
    PetscInt j,k,n = bc->count;
    for (k=0; k<n; k++) {
      PetscInt c = bc->field[k];
      PetscInt idx = a*dof + c;
      PetscScalar val = bc->value[k];
      if (iga->fixtable) val = iga->fixtableU[c + element->mapping[a]*dof];
      for (j=0; j<count; j++)
        if (index[j] == idx) break;
      if (j == count) count++;
      index[j] = idx;
      value[j] = val;
    }
    element->nfix = count;
  }
}

static void AddFlux(IGAElement element,IGAFormBC bc,PetscInt a,PetscReal A)
{
  if (bc->count) {
    PetscInt dof = element->dof;
    PetscInt count = element->nflux;
    PetscInt *index = element->iflux;
    PetscScalar *value = element->vflux;
    PetscInt j,k,n = bc->count;
    for (k=0; k<n; k++) {
      PetscInt idx = a*dof + bc->field[k];
      PetscScalar val = bc->value[k];
      for (j=0; j<count; j++)
        if (index[j] == idx) break;
      if (j == count) value[count++] = 0.0;
      index[j]  = idx;
      value[j] += val*A;
    }
    element->nflux = count;
  }
}

static void BuildFix(IGAElement element,PetscInt dir,PetscInt side)
{
  IGAForm   form = element->parent->form;
  IGAFormBC bcv  = form->value[dir][side];
  IGAFormBC bcl  = form->load [dir][side];
  if (bcv->count || bcl->count) {
    PetscReal Area = bcl->count ? BoundaryArea(element,dir,side) : 1.0;
    IGABasis *BD = element->parent->basis;
    PetscInt S[3]={0,0,0},E[3]={1,1,1};
    PetscInt ia,ja,ka,jstride,kstride,a;
    PetscInt i,dim = element->dim;
    for (i=0; i<dim; i++) E[i] = BD[i]->nen;
    jstride = E[0]; kstride = E[0]*E[1];
    if (side) S[dir] = E[dir]-1;
    else      E[dir] = S[dir]+1;
    for (ka=S[2]; ka<E[2]; ka++)
      for (ja=S[1]; ja<E[1]; ja++)
        for (ia=S[0]; ia<E[0]; ia++)
          {
            a = ia + ja*jstride + ka*kstride;
            AddFixa(element,bcv,a);
            AddFlux(element,bcl,a,Area);
          }
  }
}

PETSC_STATIC_INLINE
IGAFormBC AtBoundaryV(IGAElement element,PetscInt dir,PetscInt side)
{
  IGAFormBC bc = element->parent->form->value[dir][side];
  PetscInt  e  = side ? element->sizes[dir]-1 : 0;
  return (element->ID[dir] == e) ? bc : NULL;
}
PETSC_STATIC_INLINE
IGAFormBC AtBoundaryL(IGAElement element,PetscInt dir,PetscInt side)
{
  IGAFormBC bc = element->parent->form->load[dir][side];
  PetscInt  e  = side ? element->sizes[dir]-1 : 0;
  return (element->ID[dir] == e) ? bc : NULL;
}

PETSC_STATIC_INLINE
PetscReal DOT(PetscInt dim,const PetscReal a[],const PetscReal b[])
{
  PetscInt i; PetscReal s = 0.0;
  for (i=0; i<dim; i++) s += a[i]*b[i];
  return s;
}

#undef  __FUNCT__
#define __FUNCT__ "IGAElementBuildFix"
PetscErrorCode IGAElementBuildFix(IGAElement element)
{
  PetscFunctionBegin;
  PetscValidPointer(element,1);
  if (PetscUnlikely(element->index < 0))
    SETERRQ(PETSC_COMM_SELF,PETSC_ERR_ARG_WRONGSTATE,"Must call during element loop");
  if (PetscUnlikely(element->collocation)) goto collocation;
  element->nfix  = 0;
  element->nflux = 0;
  {
    IGAAxis  *AX = element->parent->axis;
    PetscInt *ID = element->ID;
    PetscInt i,dim = element->dim;
    for (i=0; i<dim; i++) {
      PetscBool w = AX[i]->periodic;
      PetscInt  e = element->sizes[i]-1; /* last element */
      if (ID[i] == 0 && !w) BuildFix(element,i,0);
      if (ID[i] == e && !w) BuildFix(element,i,1);
    }
  }
  PetscFunctionReturn(0);
 collocation:
  element->nfix  = 0;
  element->nflux = 0;
  {
    PetscInt L[3] = {PETSC_MIN_INT,PETSC_MIN_INT,PETSC_MIN_INT};
    PetscInt R[3] = {PETSC_MAX_INT,PETSC_MAX_INT,PETSC_MAX_INT};
    {
      IGAAxis  *AX = element->parent->axis;
      PetscInt i,dim = element->dim;
      for (i=0; i<dim; i++) {
        PetscBool w = AX[i]->periodic;
        PetscInt  n = element->sizes[i]-1; /* last node */
        L[i] = 0; if (!w) R[i] = n;
      }
    }
    {
      IGAFormBC (*bc)[2] = element->parent->form->value;
      IGABasis *BD = element->parent->basis;
      PetscInt *ID = element->ID;
      PetscInt ia, inen = BD[0]->nen, ioffset = BD[0]->offset[ID[0]];
      PetscInt ja, jnen = BD[1]->nen, joffset = BD[1]->offset[ID[1]];
      PetscInt ka, knen = BD[2]->nen, koffset = BD[2]->offset[ID[2]];
      PetscInt a = 0;
      for (ka=0; ka<knen; ka++)
        for (ja=0; ja<jnen; ja++)
          for (ia=0; ia<inen; ia++)
            {
              PetscInt iA = ioffset + ia;
              PetscInt jA = joffset + ja;
              PetscInt kA = koffset + ka;
              /**/ if (iA == L[0]) AddFixa(element,bc[0][0],a);
              else if (iA == R[0]) AddFixa(element,bc[0][1],a);
              /**/ if (jA == L[1]) AddFixa(element,bc[1][0],a);
              else if (jA == R[1]) AddFixa(element,bc[1][1],a);
              /**/ if (kA == L[2]) AddFixa(element,bc[2][0],a);
              else if (kA == R[2]) AddFixa(element,bc[2][1],a);
              a++;
            }
    }
  }
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGAElementFixValues"
PetscErrorCode IGAElementFixValues(IGAElement element,PetscScalar U[])
{
  PetscFunctionBegin;
  PetscValidPointer(element,1);
  PetscValidScalarPointer(U,2);
  {
    PetscInt f,n;
    n = element->nfix;
    for (f=0; f<n; f++) {
      PetscInt k = element->ifix[f];
      element->ufix[f] = U[k];
      U[k] = element->vfix[f];
    }
  }
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGAElementFixSystem"
PetscErrorCode IGAElementFixSystem(IGAElement element,PetscScalar K[],PetscScalar F[])
{
  PetscFunctionBegin;
  PetscValidPointer(element,1);
  PetscValidScalarPointer(K,2);
  PetscValidScalarPointer(F,3);
  if (PetscUnlikely(element->collocation)) goto collocation;
  {
    PetscInt M = element->neq * element->dof;
    PetscInt N = element->nen * element->dof;
    PetscInt f,n;
    n = element->nflux;
    for (f=0; f<n; f++) {
      PetscInt    k = element->iflux[f];
      PetscScalar v = element->vflux[f];
      F[k] += v;
    }
    n = element->nfix;
    for (f=0; f<n; f++) {
      PetscInt    k = element->ifix[f];
      PetscScalar v = element->vfix[f];
      PetscInt i,j;
      for (i=0; i<M; i++) F[i] -= K[i*N+k] * v;
      for (i=0; i<M; i++) K[i*N+k] = 0.0;
      for (j=0; j<N; j++) K[k*N+j] = 0.0;
      K[k*N+k] = 1.0;
      F[k]     = v;
    }
  }
  PetscFunctionReturn(0);
 collocation:
  {
    PetscInt dim = element->dim;
    PetscInt dof = element->dof;
    PetscInt nen = element->nen;
    PetscInt N = nen * dof;
    PetscInt dir,side;
    for (dir=0; dir<dim; dir++) {
      for (side=0; side<2; side++) {
        IGAFormBC bcl = AtBoundaryL(element,dir,side);
        IGAFormBC bcv = AtBoundaryV(element,dir,side);
        if (bcl && bcl->count) {
          PetscInt  f, n = bcl->count;
          PetscReal *dshape, normal[3] = {0.0,0.0,0.0};
          if (!element->geometry) {
            normal[dir] = side ? 1.0 : -1.0;
            dshape = element->basis[1];
          } else {
            PetscReal dS, *F = element->gradX[0];
            IGA_GetNormal(dim,dir,side,F,&dS,normal);
            dshape = element->shape[1];
          }
          for (f=0; f<n; f++) {
            PetscInt    c = bcl->field[f];
            PetscScalar v = bcl->value[f];
            PetscInt    a,j;
            for (j=0; j<N; j++) K[c*N+j] = 0.0;
            for (a=0; a<nen; a++)
              K[c*N+a*dof+c] = DOT(dim,&dshape[a*dim],normal);
            F[c] = v;
          }
        }
        if (bcv && bcv->count) {
          PetscInt  f, n = bcv->count;
          PetscReal *shape = element->basis[0];
          for (f=0; f<n; f++) {
            PetscInt    c = bcv->field[f];
            PetscScalar v = bcv->value[f];
            PetscInt    a,j;
            for (j=0; j<N; j++) K[c*N+j] = 0.0;
            for (a=0; a<nen; a++)
              K[c*N+a*dof+c] = shape[a];
            F[c] = v;
          }
        }
      }
    }
  }
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGAElementFixFunction"
PetscErrorCode IGAElementFixFunction(IGAElement element,PetscScalar F[])
{
  PetscFunctionBegin;
  PetscValidPointer(element,1);
  PetscValidScalarPointer(F,2);
  if (PetscUnlikely(element->collocation)) goto collocation;
  {
    PetscInt f,n;
    n = element->nflux;
    for (f=0; f<n; f++) {
      PetscInt    k = element->iflux[f];
      PetscScalar v = element->vflux[f];
      F[k] -= v;
    }
    n = element->nfix;
    for (f=0; f<n; f++) {
      PetscInt    k = element->ifix[f];
      PetscScalar v = element->vfix[f];
      PetscScalar u = element->ufix[f];
      F[k] = u - v;
    }
  }
  PetscFunctionReturn(0);
 collocation:
  {
    PetscInt f,n;
    n = element->nfix;
    for (f=0; f<n; f++) {
      PetscInt k = element->ifix[f];
      PetscInt a = k / element->dof;
      PetscInt c = k % element->dof;
      if (element->rowmap[0] == element->colmap[a])
        {
          PetscScalar v = element->vfix[f];
          PetscScalar u = element->ufix[f];
          F[c] = u - v;
        }
    }
  }
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGAElementFixJacobian"
PetscErrorCode IGAElementFixJacobian(IGAElement element,PetscScalar J[])
{
  PetscFunctionBegin;
  PetscValidPointer(element,1);
  PetscValidScalarPointer(J,2);
  if (PetscUnlikely(element->collocation)) goto collocation;
  {
    PetscInt M = element->neq * element->dof;
    PetscInt N = element->nen * element->dof;
    PetscInt f,n;
    n = element->nfix;
    for (f=0; f<n; f++) {
      PetscInt i,j,k=element->ifix[f];
      for (i=0; i<M; i++) J[i*N+k] = 0.0;
      for (j=0; j<N; j++) J[k*N+j] = 0.0;
      J[k*N+k] = 1.0;
    }
  }
  PetscFunctionReturn(0);
 collocation:
  {
    PetscInt nen = element->nen;
    PetscInt dof = element->dof;
    PetscInt f,n;
    n = element->nfix;
    for (f=0; f<n; f++) {
      PetscInt k = element->ifix[f];
      PetscInt a = k / dof;
      PetscInt c = k % dof;
      if (element->rowmap[0] == element->colmap[a])
        {
          PetscInt  i,j,N=nen*dof;
          PetscReal *shape = element->basis[0];
          for (j=0; j<N; j++) J[c*N+j] = 0.0;
          for (i=0; i<nen; i++)
            J[c*N+i*dof+c] = shape[i];
        }
    }
  }
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGAElementAssembleVec"
PetscErrorCode IGAElementAssembleVec(IGAElement element,const PetscScalar F[],Vec vec)
{
  PetscInt       mm,*ii;
  PetscErrorCode ierr;
  PetscFunctionBegin;
  PetscValidPointer(element,1);
  PetscValidScalarPointer(F,2);
  PetscValidHeaderSpecific(vec,VEC_CLASSID,3);
  mm = element->neq; ii = element->rowmap;
  if (element->dof == 1) {
    ierr = VecSetValuesLocal(vec,mm,ii,F,ADD_VALUES);CHKERRQ(ierr);
  } else {
    ierr = VecSetValuesBlockedLocal(vec,mm,ii,F,ADD_VALUES);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGAElementAssembleMat"
PetscErrorCode IGAElementAssembleMat(IGAElement element,const PetscScalar K[],Mat mat)
{
  PetscInt       mm,*ii;
  PetscInt       nn,*jj;
  PetscErrorCode ierr;
  PetscFunctionBegin;
  PetscValidPointer(element,1);
  PetscValidScalarPointer(K,2);
  PetscValidHeaderSpecific(mat,MAT_CLASSID,3);
  mm = element->neq; ii = element->rowmap;
  nn = element->nen; jj = element->colmap;
  if (element->dof == 1) {
    ierr = MatSetValuesLocal(mat,mm,ii,nn,jj,K,ADD_VALUES);CHKERRQ(ierr);
  } else {
    ierr = MatSetValuesBlockedLocal(mat,mm,ii,nn,jj,K,ADD_VALUES);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

/* -------------------------------------------------------------------------- */
