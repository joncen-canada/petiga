#include "petiga.h"

PetscClassId IGA_CLASSID = 0;

static PetscBool IGAPackageInitialized = PETSC_FALSE;
PetscBool IGARegisterAllCalled = PETSC_FALSE;
PetscLogEvent IGA_Event = 0;

#undef  __FUNCT__
#define __FUNCT__ "IGARegisterAll"
PetscErrorCode IGARegisterAll(PETSC_UNUSED const char path[])
{
  /*PetscErrorCode ierr;*/
  PetscFunctionBegin;
  IGARegisterAllCalled = PETSC_TRUE;
  PetscFunctionReturn(0);
}

#undef  __FUNCT__
#define __FUNCT__ "IGAFinalizePackage"
PetscErrorCode IGAFinalizePackage(void)
{
  /*PetscErrorCode ierr;*/
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
  ierr = PetscLogEventRegister("IGAEvent",IGA_CLASSID,&IGA_Event);CHKERRQ(ierr);
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
