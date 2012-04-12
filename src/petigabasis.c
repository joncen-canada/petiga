#include "petiga.h"

#undef  __FUNCT__
#define __FUNCT__ "IGABasisCreate"
PetscErrorCode IGABasisCreate(IGABasis *basis)
{
  PetscErrorCode ierr;
  PetscFunctionBegin;
  PetscValidPointer(basis,3);
  ierr = PetscNew(struct _n_IGABasis,basis);CHKERRQ(ierr);
  (*basis)->refct = 1;
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGABasisDestroy"
PetscErrorCode IGABasisDestroy(IGABasis *_basis)
{
  IGABasis       basis;
  PetscErrorCode ierr;
  PetscFunctionBegin;
  PetscValidPointer(_basis,1);
  basis = *_basis; *_basis = 0;
  if (!basis) PetscFunctionReturn(0);
  if (--basis->refct > 0) PetscFunctionReturn(0);
  ierr = IGABasisReset(basis);CHKERRQ(ierr);
  ierr = PetscFree(basis);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGABasisReset"
PetscErrorCode IGABasisReset(IGABasis basis)
{
  PetscErrorCode ierr;
  PetscFunctionBegin;
  if (!basis) PetscFunctionReturn(0);
  PetscValidPointer(basis,1);
  basis->nel = 0;
  basis->nqp = 0;
  basis->nen = 0;
  basis->p   = 0;
  basis->d   = 0;
  ierr = PetscFree(basis->detJ);CHKERRQ(ierr);
  ierr = PetscFree(basis->weight);CHKERRQ(ierr);
  ierr = PetscFree(basis->point);CHKERRQ(ierr);
  ierr = PetscFree(basis->value);CHKERRQ(ierr);
  basis->periodic = PETSC_FALSE;
  basis->nnp = 0;
  ierr = PetscFree(basis->span);CHKERRQ(ierr);
  ierr = PetscFree(basis->offset);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGABasisReference"
PetscErrorCode IGABasisReference(IGABasis basis)
{
  PetscFunctionBegin;
  PetscValidPointer(basis,1);
  basis->refct++;
  PetscFunctionReturn(0);
}

EXTERN_C_BEGIN
static PetscInt SpanCount(PetscInt n, PetscInt p, PetscReal U[]);
static PetscInt SpanIndex(PetscInt n, PetscInt p, PetscReal U[],PetscInt index[]);
extern void IGA_DersBasisFuns(PetscInt i,PetscReal u,PetscInt p,PetscInt d,const PetscReal U[],PetscReal N[]);
EXTERN_C_END

#undef  __FUNCT__
#define __FUNCT__ "IGABasisInit"
PetscErrorCode IGABasisInit(IGABasis basis,IGAAxis axis,IGARule rule,PetscInt d)
{
  PetscBool      periodic;
  PetscInt       p,m,n,s;
  PetscReal      *U,*X,*W;
  PetscInt       iel,nel;
  PetscInt       iqp,nqp;
  PetscInt       nen,ndr;
  PetscInt       *span;
  PetscReal      *detJ;
  PetscReal      *weight;
  PetscReal      *point;
  PetscReal      *value;
  PetscInt       nnp;
  PetscInt       *offset;
  PetscErrorCode ierr;
  PetscFunctionBegin;
  PetscValidPointer(basis,1);
  PetscValidPointer(axis,2);
  PetscValidPointer(rule,3);
  if (d < 0) SETERRQ1(PETSC_COMM_SELF,PETSC_ERR_ARG_OUTOFRANGE,
                      "Derivative order must be grather than zero, got %D",d);

  p = axis->p;
  m = axis->m;
  U = axis->U;
  n = m-p-1;

  nqp = rule->nqp;
  X   = rule->point;
  W   = rule->weight;

  nel = SpanCount(n,p,U);
  nen = p+1;
  ndr = d+1;

  periodic = axis->periodic;
  for (s=1; s<p && U[m-p] == U[m-p+s]; s++);
  nnp = periodic ? n-p+s : n+1;

  ierr = PetscMalloc1(nel,PetscInt,&span);CHKERRQ(ierr);
  ierr = PetscMalloc1(nel,PetscReal,&detJ);CHKERRQ(ierr);
  ierr = PetscMalloc1(nqp,PetscReal,&weight);CHKERRQ(ierr);
  ierr = PetscMalloc1(nel*nqp,PetscReal,&point);CHKERRQ(ierr);
  ierr = PetscMalloc1(nel*nqp*nen*ndr,PetscReal,&value);CHKERRQ(ierr);
  ierr = PetscMalloc1(nel,PetscInt,&offset);CHKERRQ(ierr);

  for (iqp=0; iqp<nqp; iqp++) {
    weight[iqp] = W[iqp];
  }
  SpanIndex(n,p,U,span);
  for (iel=0; iel<nel; iel++) {
    PetscInt  k = span[iel];
    PetscReal u0 = U[k], u1 = U[k+1];
    PetscReal J = (u1-u0)/2;
    PetscReal *u = &point[iel*nqp];
    PetscReal *N = &value[iel*nqp*nen*ndr];
    detJ[iel] = J;
    for (iqp=0; iqp<nqp; iqp++) {
      u[iqp] = (X[iqp] + 1) * J + u0;
      IGA_DersBasisFuns(k,u[iqp],p,d,U,&N[iqp*nen*ndr]);
    }
    offset[iel] = k-p;
  }

  ierr = IGABasisReset(basis);CHKERRQ(ierr);

  basis->nel    = nel;
  basis->nqp    = nqp;
  basis->nen    = nen;
  basis->p      = p;
  basis->d      = d;
  basis->detJ   = detJ;
  basis->weight = weight;
  basis->point  = point;
  basis->value  = value;

  basis->periodic = periodic;
  basis->nnp      = nnp;
  basis->span     = span;
  basis->offset   = offset;

  PetscFunctionReturn(0);
}

static PetscInt SpanCount(PetscInt n, PetscInt p, PetscReal U[])
{
  PetscInt i, span = 0;
  for (i=p; i<=n; i++)
    if (U[i] != U[i+1])
      span++;
  return span;
}

static PetscInt SpanIndex(PetscInt n, PetscInt p, PetscReal U[],PetscInt index[])
{
  PetscInt i, span = 0;
  for (i=p; i<=n; i++)
    if (U[i] != U[i+1])
      index[span++] = i;
  return span;
}
