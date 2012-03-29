#include "petiga.h"

typedef struct { 
  PetscReal theta,cbar,alpha;
  PetscReal L0,lambda;
} AppCtx;

#undef  __FUNCT__
#define __FUNCT__ "Mobility"
void Mobility(AppCtx *user,PetscReal c,PetscReal *M,PetscReal *dM,PetscReal *d2M)
{
  if (M)   *M   = c*(1-c);
  if (dM)  *dM  = 1-2*c;
  if (d2M) *d2M = -2;
}

#undef  __FUNCT__
#define __FUNCT__ "ChemicalPotential"
void ChemicalPotential(AppCtx *user,PetscReal c,PetscReal *mu,PetscReal *dmu,PetscReal *d2mu)
{
  if (mu) {
    (*mu)  = 0.5/user->theta*log(c/(1-c))+1-2*c;
    (*mu) *= user->L0*user->L0/user->lambda;
  }
  if (dmu) {
    (*dmu)  = 0.5/user->theta*1.0/(c*(1-c)) - 2;
    (*dmu) *= user->L0*user->L0/user->lambda;
  }
  if (d2mu) {
    (*d2mu)  = -0.5/user->theta*(1-2*c)/(c*c*(1-c)*(1-c));
    (*d2mu) *= user->L0*user->L0/user->lambda;
  }
}

#undef  __FUNCT__
#define __FUNCT__ "Residual"
PetscErrorCode Residual(IGAPoint p,PetscReal dt,PetscReal shift,
                        PetscReal t,const PetscScalar *V,const PetscScalar *U,
                        PetscScalar *R,void *ctx)
{
  AppCtx *user = (AppCtx *)ctx;

  PetscScalar c_t,c;
  IGAPointInterpolate(p,0,V,&c_t);
  IGAPointInterpolate(p,0,U,&c);

  PetscReal M,dM;
  Mobility(user,c,&M,&dM,NULL);
  PetscReal dmu;
  ChemicalPotential(user,c,NULL,&dmu,NULL);

  PetscScalar c1[2],c2[2][2];
  IGAPointInterpolate(p,1,U,&c1[0]);
  IGAPointInterpolate(p,2,U,&c2[0][0]);
  PetscScalar c_x  = c1[0],    c_y  = c1[1];
  PetscScalar c_xx = c2[0][0], c_yy = c2[1][1];

  PetscReal *N0 = p->shape[0];
  PetscReal (*N1)[2] = (PetscReal (*)[2]) p->shape[1];
  PetscReal (*N2)[2][2] = (PetscReal (*)[2][2]) p->shape[2];

  PetscInt a,nen=p->nen;
  for (a=0; a<nen; a++) {
    PetscReal Na    = N0[a];
    PetscReal Na_x  = N1[a][0];
    PetscReal Na_y  = N1[a][1];
    PetscReal Na_xx = N2[a][0][0];
    PetscReal Na_yy = N2[a][1][1];
    /* ----- */
    PetscScalar Ra  = 0;
    // Na * c_t
    Ra += Na * c_t; 
    // grad(Na) . ((M*dmu + dM*del2(c))) grad(C)
    PetscScalar t1 = M*dmu + dM*(c_xx+c_yy);
    Ra += Na_x * t1 * c_x;
    Ra += Na_y * t1 * c_y;
    // del2(Na) * M * del2(c)
    Ra += (Na_xx+Na_yy) * M * (c_xx+c_yy);
    /* ----- */
    R[a] = Ra;
  }
  return 0;
}

#undef  __FUNCT__
#define __FUNCT__ "Tangent"
PetscErrorCode Tangent(IGAPoint p,PetscReal dt,PetscReal shift,
                       PetscReal t,const PetscScalar *V,const PetscScalar *U,
                       PetscScalar *K,void *ctx)
{
  AppCtx *user = (AppCtx *)ctx;

  PetscScalar c_t,c;
  IGAPointInterpolate(p,0,V,&c_t);
  IGAPointInterpolate(p,0,U,&c);

  PetscReal M,dM,d2M;
  Mobility(user,c,&M,&dM,&d2M);
  PetscReal dmu,d2mu;
  ChemicalPotential(user,c,NULL,&dmu,&d2mu);

  PetscScalar c1[2],c2[2][2];
  IGAPointInterpolate(p,1,U,&c1[0]);
  IGAPointInterpolate(p,2,U,&c2[0][0]);
  PetscScalar c_x  = c1[0],    c_y  = c1[1];
  PetscScalar c_xx = c2[0][0], c_yy = c2[1][1];

  PetscReal *N0 = p->shape[0];
  PetscReal (*N1)[2] = (PetscReal (*)[2]) p->shape[1];
  PetscReal (*N2)[2][2] = (PetscReal (*)[2][2]) p->shape[2];

  PetscInt a,b,nen=p->nen;
  for (a=0; a<nen; a++) {
    PetscReal Na    = N0[a];
    PetscReal Na_x  = N1[a][0];
    PetscReal Na_y  = N1[a][1];
    PetscReal Na_xx = N2[a][0][0];
    PetscReal Na_yy = N2[a][1][1];
    for (b=0; b<nen; b++) {
      PetscReal Nb    = N0[b];
      PetscReal Nb_x  = N1[b][0];
      PetscReal Nb_y  = N1[b][1];
      PetscReal Nb_xx = N2[b][0][0];
      PetscReal Nb_yy = N2[b][1][1];
      /* ----- */
      PetscScalar Kab = 0;
      // shift*Na*Nb
      Kab += shift*Na*Nb;
      // grad(Na) . (M*dmu+dM*del2(c)) grad(Nb)
      PetscScalar t1 = M*dmu + dM*(c_xx+c_yy);
      Kab += Na_x * t1 * Nb_x;
      Kab += Na_y * t1 * Nb_y;
      // grad(Na) . ((dM*dmu+M*d2mu+d2M*del2(c))*Nb + dM*del2(Nb)) grad(C)
      PetscScalar t2 = (dM*dmu+M*d2mu+d2M*(c_xx+c_yy))*Nb + dM*(Nb_xx+Nb_yy);
      Kab += Na_x * t2 * c_x;
      Kab += Na_y * t2 * c_y;
      // del2(Na) * ((dM*del2(c)*Nb + M*del2(Nb))
      Kab += (Na_xx+Na_yy) * (dM*(c_xx+c_yy)*Nb + M*(Nb_xx+Nb_yy));
      /* ----- */
      K[a*nen+b] = Kab;
    }
  }
  return 0;
}

#undef __FUNCT__
#define __FUNCT__ "FormInitialCondition"
PetscErrorCode FormInitialCondition(AppCtx *user,IGA iga,const char datafile[],Vec C)
{
  
  PetscErrorCode ierr;
  PetscFunctionBegin;
  if (datafile[0] != 0) { /* initial condition from datafile */
    MPI_Comm comm;
    PetscViewer viewer;
    ierr = PetscObjectGetComm((PetscObject)C,&comm);CHKERRQ(ierr);
    ierr = PetscViewerBinaryOpen(comm,datafile,FILE_MODE_READ,&viewer);CHKERRQ(ierr);
    ierr = VecLoad(C,viewer);CHKERRQ(ierr);
    ierr = PetscViewerDestroy(&viewer);
  } else { /* initial condition is random */
    PetscRandom rctx;    
    ierr = PetscRandomCreate(PETSC_COMM_WORLD,&rctx);CHKERRQ(ierr);
    ierr = PetscRandomSetFromOptions(rctx);CHKERRQ(ierr);
    ierr = PetscRandomSetInterval(rctx,user->cbar-0.05,user->cbar+0.05);CHKERRQ(ierr); 
    ierr = PetscRandomSeed(rctx);CHKERRQ(ierr);
    ierr = VecSetRandom(C,rctx);CHKERRQ(ierr); 
    ierr = PetscRandomDestroy(&rctx);CHKERRQ(ierr); 
  }
  PetscFunctionReturn(0); 
}

#undef __FUNCT__
#define __FUNCT__ "WriteSolution"
PetscErrorCode WriteSolution(Vec C, const char pattern[],int number)
{
  PetscFunctionBegin;
  PetscErrorCode  ierr;
  MPI_Comm        comm;
  char            filename[256];
  PetscViewer     viewer;

  PetscFunctionBegin;
  sprintf(filename,pattern,number);
  ierr = PetscObjectGetComm((PetscObject)C,&comm);CHKERRQ(ierr);
  ierr = PetscViewerBinaryOpen(comm,filename,FILE_MODE_WRITE,&viewer);CHKERRQ(ierr);
  ierr = VecView(C,viewer);CHKERRQ(ierr);
  ierr = PetscViewerFlush(viewer);CHKERRQ(ierr);
  ierr = PetscViewerDestroy(&viewer);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}
#undef __FUNCT__
#define __FUNCT__ "OutputMonitor"
PetscErrorCode OutputMonitor(TS ts,PetscInt step,PetscReal t,Vec U,void *mctx)
{
  PetscErrorCode ierr;
  PetscFunctionBegin;
  ierr = WriteSolution(U,"ch%d.dat",step);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}


#undef __FUNCT__
#define __FUNCT__ "main"
int main(int argc, char *argv[]) {

  PetscErrorCode  ierr;
  ierr = PetscInitialize(&argc,&argv,0,0);CHKERRQ(ierr);

  /* Define simulation specific parameters */
  AppCtx user;
  user.cbar  = 0.63;   /* average concentration */
  user.alpha = 3000.0; /* thickess interface parameter */
  user.theta = 1.5;    /* temperature/critical temperature */
  user.L0    = 1.0;    /* length scale */

  /* Set discretization options */
  PetscInt N=64, p=2, C=PETSC_DECIDE;
  PetscBool output = PETSC_FALSE; 
  PetscBool monitor = PETSC_FALSE; 
  char initial[PETSC_MAX_PATH_LEN] = {0};
  ierr = PetscOptionsBegin(PETSC_COMM_WORLD,"","CahnHilliard2D Options","IGA");CHKERRQ(ierr);
  ierr = PetscOptionsInt("-N","number of elements (along one dimension)",__FILE__,N,&N,PETSC_NULL);CHKERRQ(ierr);
  ierr = PetscOptionsInt("-p","polynomial order",__FILE__,p,&p,PETSC_NULL);CHKERRQ(ierr);
  ierr = PetscOptionsInt("-C","global continuity order",__FILE__,C,&C,PETSC_NULL);CHKERRQ(ierr);
  ierr = PetscOptionsString("-ch_initial","Load initial solution from file",__FILE__,initial,initial,sizeof(initial),PETSC_NULL);CHKERRQ(ierr);
  ierr = PetscOptionsBool("-ch_output","Enable output files",__FILE__,output,&output,PETSC_NULL);CHKERRQ(ierr);
  ierr = PetscOptionsBool("-ch_monitor","Compute and show statistics of solution",__FILE__,monitor,&monitor,PETSC_NULL);CHKERRQ(ierr);
  ierr = PetscOptionsReal("-ch_cbar","Initial average concentration",__FILE__,user.cbar,&user.cbar,PETSC_NULL);CHKERRQ(ierr);
  ierr = PetscOptionsReal("-ch_alpha","Characteristic parameter",__FILE__,user.alpha,&user.alpha,PETSC_NULL);CHKERRQ(ierr);
  ierr = PetscOptionsEnd();CHKERRQ(ierr);
  if (C == PETSC_DECIDE) C = p-1;

  user.lambda = 1.0/N/N; /* mesh size parameter */
  
  if (p < 2 || C < 1) /* Problem requires a p>=2 C1 basis */
    SETERRQ(PETSC_COMM_WORLD,PETSC_ERR_ARG_OUTOFRANGE,
            "Problem requires minimum of p = 2 and C = 1");
  if (p <= C)         /* Check C < p */
    SETERRQ(PETSC_COMM_WORLD,PETSC_ERR_ARG_OUTOFRANGE,
            "Discretization inconsistent: polynomial order must be greater than degree of continuity");

  IGA iga;
  ierr = IGACreate(PETSC_COMM_WORLD,&iga);CHKERRQ(ierr);
  ierr = IGASetDim(iga,2);CHKERRQ(ierr);
  ierr = IGASetDof(iga,1);CHKERRQ(ierr);

  IGAAxis axis0;
  ierr = IGAGetAxis(iga,0,&axis0);CHKERRQ(ierr);
  ierr = IGAAxisSetPeriodic(axis0,PETSC_TRUE);CHKERRQ(ierr);
  ierr = IGAAxisInitUniform(axis0,p,C,N,0.0,1.0);CHKERRQ(ierr);
  IGAAxis axis1;
  ierr = IGAGetAxis(iga,1,&axis1);CHKERRQ(ierr);
  ierr = IGAAxisCopy(axis0,axis1);CHKERRQ(ierr);

  ierr = IGASetFromOptions(iga);CHKERRQ(ierr);
  ierr = IGASetUp(iga);CHKERRQ(ierr);

  ierr = IGASetUserIFunction(iga,Residual,&user);CHKERRQ(ierr);
  ierr = IGASetUserIJacobian(iga,Tangent,&user);CHKERRQ(ierr);


  TS ts;
  ierr = IGACreateTS(iga,&ts);CHKERRQ(ierr);
  ierr = TSSetDuration(ts,10000,1.0);CHKERRQ(ierr);
  ierr = TSSetTimeStep(ts,1e-10);CHKERRQ(ierr);

  ierr = TSSetType(ts,TSALPHA);CHKERRQ(ierr);
  ierr = TSAlphaSetRadius(ts,0.5);CHKERRQ(ierr);
  ierr = TSAlphaSetAdapt(ts,TSAlphaAdaptDefault,PETSC_NULL);CHKERRQ(ierr); 

  if (output) {
    ierr = TSMonitorSet(ts,OutputMonitor,&user,PETSC_NULL);CHKERRQ(ierr);
  }
  ierr = TSSetFromOptions(ts);CHKERRQ(ierr);

  PetscReal t; Vec U;
  ierr = IGACreateVec(iga,&U);CHKERRQ(ierr);
  ierr = FormInitialCondition(&user,iga,initial,U);CHKERRQ(ierr);
  ierr = TSSolve(ts,U,&t);CHKERRQ(ierr);

  ierr = VecDestroy(&U);CHKERRQ(ierr);
  ierr = TSDestroy(&ts);CHKERRQ(ierr);
  ierr = IGADestroy(&iga);CHKERRQ(ierr);
  ierr = PetscFinalize();CHKERRQ(ierr);
  return 0;
}
