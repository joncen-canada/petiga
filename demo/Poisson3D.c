#include "petiga.h"

#undef  __FUNCT__
#define __FUNCT__ "System"
PetscErrorCode System(IGAPoint p,PetscScalar *K,PetscScalar *F,void *ctx)
{
  PetscReal *N0 = p->shape[0];
  PetscReal (*N1)[3] = (PetscReal (*)[3]) p->shape[1];
  PetscInt a,b,nen=p->nen;
  for (a=0; a<nen; a++) {
    PetscReal Na   = N0[a];
    PetscReal Na_x = N1[a][0];
    PetscReal Na_y = N1[a][1];
    PetscReal Na_z = N1[a][2];
    for (b=0; b<nen; b++) {
      PetscReal Nb_x = N1[b][0];
      PetscReal Nb_y = N1[b][1];
      PetscReal Nb_z = N1[b][2];
      K[a*nen+b] = Na_x*Nb_x + Na_y*Nb_y + Na_z*Nb_z;
    }
    F[a] = Na * 1.0;
  }
  return 0;
}

#undef __FUNCT__
#define __FUNCT__ "main"
int main(int argc, char *argv[]) {

  PetscErrorCode  ierr;
  ierr = PetscInitialize(&argc,&argv,0,0);CHKERRQ(ierr);

  PetscInt N=16, p=2, C=PETSC_DECIDE;
  ierr = PetscOptionsBegin(PETSC_COMM_WORLD,"","Poisson3D Options","IGA");CHKERRQ(ierr);
  ierr = PetscOptionsInt("-N","number of elements (along one dimension)",__FILE__,N,&N,PETSC_NULL);CHKERRQ(ierr);
  ierr = PetscOptionsInt("-p","polynomial order",__FILE__,p,&p,PETSC_NULL);CHKERRQ(ierr);
  ierr = PetscOptionsInt("-C","global continuity order",__FILE__,C,&C,PETSC_NULL);CHKERRQ(ierr);
  ierr = PetscOptionsEnd();CHKERRQ(ierr);
  if (C == PETSC_DECIDE) C = p-1;

  IGA iga;
  ierr = IGACreate(PETSC_COMM_WORLD,&iga);CHKERRQ(ierr);
  ierr = IGASetDim(iga,3);CHKERRQ(ierr);
  ierr = IGASetDof(iga,1);CHKERRQ(ierr);
  
  IGAAxis axis0;
  ierr = IGAGetAxis(iga,0,&axis0);CHKERRQ(ierr);
  ierr = IGAAxisInitUniform(axis0,p,C,N,-1.0,1.0);CHKERRQ(ierr);
  IGAAxis axis1;
  ierr = IGAGetAxis(iga,1,&axis1);CHKERRQ(ierr);
  ierr = IGAAxisCopy(axis0,axis1);CHKERRQ(ierr);
  IGAAxis axis2;
  ierr = IGAGetAxis(iga,2,&axis2);CHKERRQ(ierr);
  ierr = IGAAxisCopy(axis1,axis2);CHKERRQ(ierr);

  IGABoundary bnd;
  PetscInt dir,side;
  PetscScalar value = 1.0;
  for (dir=0; dir<3; dir++) {
    for (side=0; side<2; side++) {
      ierr = IGAGetBoundary(iga,dir,side,&bnd);CHKERRQ(ierr);
      ierr = IGABoundarySetValue(bnd,0,value);CHKERRQ(ierr);
    }
  }

  ierr = IGASetUp(iga);CHKERRQ(ierr);

  Mat A;
  Vec x,b;
  ierr = IGACreateMat(iga,&A);CHKERRQ(ierr);
  ierr = IGACreateVec(iga,&x);CHKERRQ(ierr);
  ierr = IGACreateVec(iga,&b);CHKERRQ(ierr);
  ierr = IGAFormSystem(iga,A,b,System,PETSC_NULL);CHKERRQ(ierr);
  
  KSP ksp;
  ierr = IGACreateKSP(iga,&ksp);CHKERRQ(ierr);
  ierr = KSPSetOperators(ksp,A,A,SAME_NONZERO_PATTERN);CHKERRQ(ierr);
  ierr = KSPSetFromOptions(ksp);CHKERRQ(ierr);
  ierr = KSPSolve(ksp,b,x);CHKERRQ(ierr);

  /*
  PetscViewer viewer = PETSC_VIEWER_BINARY_WORLD;
  ierr = VecView(x,viewer);CHKERRQ(ierr);
  */
  
  ierr = KSPDestroy(&ksp);CHKERRQ(ierr);
  ierr = MatDestroy(&A);CHKERRQ(ierr);
  ierr = VecDestroy(&x);CHKERRQ(ierr);
  ierr = VecDestroy(&b);CHKERRQ(ierr);
  ierr = IGADestroy(&iga);CHKERRQ(ierr);

  ierr = PetscFinalize();CHKERRQ(ierr);
  return 0;
}
