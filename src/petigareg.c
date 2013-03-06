#include "petiga.h"

PETSC_EXTERN PetscLogEvent IGA_FormScalar;
PETSC_EXTERN PetscLogEvent IGA_FormSystem;
PETSC_EXTERN PetscLogEvent IGA_FormFunction;
PETSC_EXTERN PetscLogEvent IGA_FormJacobian;

EXTERN_C_BEGIN
extern PetscErrorCode PCCreate_EBE(PC);
extern PetscErrorCode PCCreate_BBB(PC);
EXTERN_C_END

EXTERN_C_BEGIN
extern PetscErrorCode TSCreate_Alpha2(TS);
EXTERN_C_END

EXTERN_C_BEGIN
extern PetscErrorCode SNESSetFromOptions_FDColoring(SNES);
EXTERN_C_END

PetscClassId IGA_CLASSID = 0;

static PetscBool IGAPackageInitialized = PETSC_FALSE;
PetscBool IGARegisterAllCalled = PETSC_FALSE;
PetscLogEvent IGA_FormScalar = 0;
PetscLogEvent IGA_FormSystem = 0;
PetscLogEvent IGA_FormFunction = 0;
PetscLogEvent IGA_FormJacobian = 0;

#undef  __FUNCT__
#define __FUNCT__ "IGARegisterAll"
PetscErrorCode IGARegisterAll(const char path[])
{
  PetscErrorCode ierr;
  PetscFunctionBegin;
  IGARegisterAllCalled = PETSC_TRUE;
  ierr = PCRegisterAll(path);CHKERRQ(ierr);
  ierr = PCRegisterDynamic(PCEBE,path,"PCCreate_EBE",PCCreate_EBE);CHKERRQ(ierr);
  ierr = PCRegisterDynamic(PCBBB,path,"PCCreate_BBB",PCCreate_BBB);CHKERRQ(ierr);
  ierr = TSRegisterAll(path);CHKERRQ(ierr);
  ierr = TSRegisterDynamic(TSALPHA2,path,"TSCreate_Alpha2",TSCreate_Alpha2);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGAFinalizePackage"
PetscErrorCode IGAFinalizePackage(void)
{
  PetscFunctionBegin;
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGAInitializePackage"
PetscErrorCode IGAInitializePackage(const char path[])
{
  PetscErrorCode ierr;
  PetscFunctionBegin;
  if (IGAPackageInitialized) PetscFunctionReturn(0);
  IGAPackageInitialized = PETSC_TRUE;
  /* Register Classes */
  ierr = PetscClassIdRegister("IGA",&IGA_CLASSID);CHKERRQ(ierr);
  /* Register Constructors */
  ierr = IGARegisterAll(path);CHKERRQ(ierr);
  /* Register Events */
  ierr = PetscLogEventRegister("IGAFormScalar",IGA_CLASSID,&IGA_FormScalar);CHKERRQ(ierr);
  ierr = PetscLogEventRegister("IGAFormSystem",IGA_CLASSID,&IGA_FormSystem);CHKERRQ(ierr);
  ierr = PetscLogEventRegister("IGAFormFunction",IGA_CLASSID,&IGA_FormFunction);CHKERRQ(ierr);
  ierr = PetscLogEventRegister("IGAFormJacobian",IGA_CLASSID,&IGA_FormJacobian);CHKERRQ(ierr);
  /* Additional option handlers */
  ierr = SNESAddOptionsChecker(SNESSetFromOptions_FDColoring);CHKERRQ(ierr);
  /* Register finalization routine */
  ierr = PetscRegisterFinalize(IGAFinalizePackage);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#ifdef PETSC_USE_DYNAMIC_LIBRARIES
EXTERN_C_BEGIN
#undef  __FUNCT__
#define __FUNCT__ "PetscDLLibraryRegister_petiga"
PetscErrorCode PetscDLLibraryRegister_petiga(const char path[])
{
  PetscErrorCode ierr;
  PetscFunctionBegin;
  ierr = IGAInitializePackage(path);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}
EXTERN_C_END
#endif
