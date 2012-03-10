#include "petiga.h"

#undef  __FUNCT__
#define __FUNCT__ "IGARuleCreate"
PetscErrorCode IGARuleCreate(IGARule *rule)
{
  PetscErrorCode ierr;
  PetscFunctionBegin;
  PetscValidPointer(rule,1);
  ierr = PetscNew(struct _n_IGARule,rule);CHKERRQ(ierr);
  (*rule)->refct = 1;
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGARuleDestroy"
PetscErrorCode IGARuleDestroy(IGARule *_rule)
{
  IGARule        rule;
  PetscErrorCode ierr;
  PetscFunctionBegin;
  PetscValidPointer(_rule,1);
  rule = *_rule; *_rule = 0;
  if (!rule) PetscFunctionReturn(0);
  if (--rule->refct > 0) PetscFunctionReturn(0);
  ierr = PetscFree(rule->point);CHKERRQ(ierr);
  ierr = PetscFree(rule->weight);CHKERRQ(ierr);
  ierr = PetscFree(rule);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGARuleReference"
PetscErrorCode IGARuleReference(IGARule rule)
{
  PetscFunctionBegin;
  PetscValidPointer(rule,1);
  rule->refct++;
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGARuleCopy"
PetscErrorCode IGARuleCopy(IGARule base,IGARule rule)
{
  PetscErrorCode ierr;
  PetscFunctionBegin;
  PetscValidPointer(base,1);
  PetscValidPointer(rule,2);
  rule->nqp = base->nqp;
  ierr = PetscFree(rule->point);CHKERRQ(ierr);
  if (base->point && base->nqp > 0) {
    ierr = PetscMalloc1(base->nqp,PetscReal,&rule->point);CHKERRQ(ierr);
    ierr = PetscMemcpy(rule->point,base->point,base->nqp*sizeof(PetscReal));CHKERRQ(ierr);
  }
  ierr = PetscFree(rule->weight);CHKERRQ(ierr);
  if (base->weight && base->nqp > 0) {
    ierr = PetscMalloc1(base->nqp,PetscReal,&rule->weight);CHKERRQ(ierr);
    ierr = PetscMemcpy(rule->weight,base->weight,base->nqp*sizeof(PetscReal));CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGARuleDuplicate"
PetscErrorCode IGARuleDuplicate(IGARule base,IGARule *rule)
{
  PetscErrorCode ierr;
  PetscFunctionBegin;
  PetscValidPointer(base,1);
  PetscValidPointer(rule,2);
  ierr = PetscNew(struct _n_IGARule,rule);CHKERRQ(ierr);
  ierr = IGARuleCopy(base,*rule);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

static PetscErrorCode GaussRule(PetscInt q, PetscReal X[], PetscReal W[]);

#undef  __FUNCT__
#define __FUNCT__ "IGARuleInit"
PetscErrorCode IGARuleInit(IGARule rule,PetscInt nqp)
{
  PetscReal      *point,*weight;
  PetscErrorCode ierr;
  PetscFunctionBegin;
  PetscValidPointer(rule,1);
  if (nqp < 1)
    SETERRQ1(PETSC_COMM_SELF,PETSC_ERR_ARG_OUTOFRANGE,
             "Number of quadrature points must be grather than zero, got %D",nqp);
  ierr = PetscMalloc1(nqp,PetscReal,&point);CHKERRQ(ierr);
  ierr = PetscMalloc1(nqp,PetscReal,&weight);CHKERRQ(ierr);
  if (GaussRule(nqp,point,weight) != 0)
    SETERRQ1(PETSC_COMM_SELF,PETSC_ERR_ARG_OUTOFRANGE,
             "Number of quadrature points %D not implemented",nqp);
  ierr = PetscFree(rule->point);CHKERRQ(ierr);
  ierr = PetscFree(rule->weight);CHKERRQ(ierr);
  rule->nqp = nqp;
  rule->point = point;
  rule->weight = weight;
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGARuleSetRule"
PetscErrorCode IGARuleSetRule(IGARule rule,PetscInt q,const PetscReal x[],const PetscReal w[])
{
  PetscReal      *xx,*ww;
  PetscErrorCode ierr;
  PetscFunctionBegin;
  PetscValidPointer(rule,1);
  PetscValidPointer(x,3);
  PetscValidPointer(w,4);
  if (q < 1)
    SETERRQ1(PETSC_COMM_SELF,PETSC_ERR_ARG_OUTOFRANGE,
             "Number of quadrature points must be grather than zero, got %D",q);
  ierr = PetscMalloc1(q,PetscReal,&xx);CHKERRQ(ierr);
  ierr = PetscMalloc1(q,PetscReal,&ww);CHKERRQ(ierr);
  ierr = PetscMemcpy(xx,x,q*sizeof(PetscReal));CHKERRQ(ierr);
  ierr = PetscMemcpy(ww,w,q*sizeof(PetscReal));CHKERRQ(ierr);
  ierr = PetscFree(rule->point);CHKERRQ(ierr);
  ierr = PetscFree(rule->weight);CHKERRQ(ierr);
  rule->nqp = q;
  rule->point = xx;
  rule->weight = ww;
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGARuleGetRule"
PetscErrorCode IGARuleGetRule(IGARule rule,PetscInt *q,PetscReal *x[],PetscReal *w[])
{
  PetscFunctionBegin;
  PetscValidPointer(rule,1);
  if (q) PetscValidPointer(q,2);
  if (x) PetscValidPointer(x,3);
  if (w) PetscValidPointer(w,4);
  if (q) *q = rule->nqp;
  if (x) *x = rule->point;
  if (w) *w = rule->weight;
  PetscFunctionReturn(0);
}

static PetscErrorCode GaussRule(PetscInt q, PetscReal X[], PetscReal W[])
{
  switch (q)  {
  case (1): /* p = 1 */
    X[0] = 0.0;
    W[0] = 2.0;
    break;
  case (2): /* p = 3 */
    X[0] = -0.5773502691896257645091487805019576; /* 1/sqrt(3) */
    X[1] = -X[0];
    W[0] =  1.0;
    W[1] =  W[0];
    break;
  case (3): /* p = 5 */
    X[0] = -0.7745966692414833770358530799564799; /* sqrt(3/5) */
    X[1] =  0.0;
    X[2] = -X[0];
    W[0] =  0.5555555555555555555555555555555556; /* 5/9 */
    W[1] =  0.8888888888888888888888888888888889; /* 8/9 */
    W[2] =  W[0];
    break;
  case (4): /* p = 7 */
    X[0] = -0.8611363115940525752239464888928094; /* sqrt((3+2*sqrt(6/5))/7) */
    X[1] = -0.3399810435848562648026657591032448; /* sqrt((3-2*sqrt(6/5))/7) */
    X[2] = -X[1];
    X[3] = -X[0];
    W[0] =  0.3478548451374538573730639492219994; /* (18-sqrt(30))/36 */
    W[1] =  0.6521451548625461426269360507780006; /* (18+sqrt(30))/36 */
    W[2] =  W[1];
    W[3] =  W[0];
    break;
  case (5): /* p = 9 */
    X[0] = -0.9061798459386639927976268782993929; /* 1/3*sqrt(5+2*sqrt(10/7)) */
    X[1] = -0.5384693101056830910363144207002086; /* 1/3*sqrt(5-2*sqrt(10/7)) */
    X[2] =  0.0;
    X[3] = -X[1];
    X[4] = -X[0];
    W[0] =  0.2369268850561890875142640407199173; /* (322-13*sqrt(70))/900 */
    W[1] =  0.4786286704993664680412915148356382; /* (322+13*sqrt(70))/900 */
    W[2] =  0.5688888888888888888888888888888889; /* 128/225 */
    W[3] =  W[1];
    W[4] =  W[0];
    break;
  case (6): /* p = 11 */
    X[0] = -0.9324695142031520278123015544939946; /* << NumericalDifferentialEquationAnalysis` */
    X[1] = -0.6612093864662645136613995950199053; /* GaussianQuadratureWeights[6, -1, 1, 37]   */
    X[2] = -0.2386191860831969086305017216807119;
    X[3] = -X[2];
    X[4] = -X[1];
    X[5] = -X[0];
    W[0] =  0.171324492379170345040296142172732894;
    W[1] =  0.360761573048138607569833513837716112;
    W[2] =  0.467913934572691047389870343989550995;
    W[3] =  W[2];
    W[4] =  W[1];
    W[5] =  W[0];
  case (7): /* p = 13 */
    X[0] = -0.9491079123427585245261896840478513; /* << NumericalDifferentialEquationAnalysis` */
    X[1] = -0.7415311855993944398638647732807884; /* GaussianQuadratureWeights[7, -1, 1, 37]   */
    X[2] = -0.4058451513773971669066064120769615;   
    X[3] =  0.0;
    X[4] = -X[2];
    X[5] = -X[1];
    X[6] = -X[0];
    W[0] =  0.129484966168869693270611432679082018;
    W[1] =  0.279705391489276667901467771423779582;
    W[2] =  0.381830050505118944950369775488975134;
    W[3] =  0.417959183673469387755102040816326531;
    W[4] =  W[2];
    W[5] =  W[1];
    W[6] =  W[0];
  case (8): /* p = 15 */
    X[0] = -0.9602898564975362316835608685694730; /* << NumericalDifferentialEquationAnalysis` */
    X[1] = -0.7966664774136267395915539364758304; /* GaussianQuadratureWeights[8, -1, 1, 37]   */
    X[2] = -0.5255324099163289858177390491892463;   
    X[3] = -0.1834346424956498049394761423601840;
    X[4] = -X[3];
    X[5] = -X[2];
    X[6] = -X[1];
    X[7] = -X[0];
    W[0] =  0.101228536290376259152531354309962190;
    W[1] =  0.222381034453374470544355994426240884;
    W[2] =  0.313706645877887287337962201986601313;
    W[3] =  0.362683783378361982965150449277195612;
    W[4] =  W[3];
    W[5] =  W[2];
    W[6] =  W[1];
    W[7] =  W[0];
  case (9): /* p = 17 */
    X[0] = -0.9681602395076260898355762029036729; /* << NumericalDifferentialEquationAnalysis` */
    X[1] = -0.8360311073266357942994297880697349; /* GaussianQuadratureWeights[9, -1, 1, 37]   */
    X[2] = -0.6133714327005903973087020393414742;   
    X[3] = -0.3242534234038089290385380146433366;
    X[4] =  0.0;
    X[5] = -X[3];
    X[6] = -X[2];
    X[7] = -X[1];
    X[8] = -X[0];
    W[0] =  0.081274388361574411971892158110523651;
    W[1] =  0.180648160694857404058472031242912810;
    W[2] =  0.260610696402935462318742869418632850;
    W[3] =  0.312347077040002840068630406584443666;
    W[4] =  0.330239355001259763164525069286974049;
    W[5] =  W[3];
    W[6] =  W[2];
    W[7] =  W[1];
    W[8] =  W[0];
  default:
    return -1;
  }
  return 0;
}
