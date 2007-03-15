/*++

Copyright (c) 2006 - 2007, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:
  Ia32Reset.c

Abstract:

--*/

#include "Reset.h"
#include "CpuIA32.h"

#include EFI_ARCH_PROTOCOL_DEFINITION (Reset)

//
// The handle onto which the Reset Architectural Protocol is installed
//
EFI_HANDLE  mResetHandle = NULL;
static EFI_ACPI_DESCRIPTION      mAcpiDescription;

VOID
EFIAPI
EfiAcpiResetSystem (
  IN EFI_RESET_TYPE   ResetType,
  IN EFI_STATUS       ResetStatus,
  IN UINTN            DataSize,
  IN CHAR16           *ResetData OPTIONAL
  )
/*++

Routine Description:

  Reset the system.

Arguments:
  
    ResetType - warm or cold
    ResetStatus - possible cause of reset
    DataSize - Size of ResetData in bytes
    ResetData - Optional Unicode string
    For details, see efiapi.h

Returns:
  Does not return if the reset takes place.

--*/
{
  AcpiResetSystem (ResetType, ResetStatus, DataSize, ResetData, &mAcpiDescription);
}

EFI_STATUS
EFIAPI
InitializeReset (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
/*++

Routine Description:

  Initialize the state information for the Reset Architectural Protocol

Arguments:

  ImageHandle of the loaded driver
  Pointer to the System Table

Returns:

  Status

  EFI_SUCCESS           - thread can be successfully created
  EFI_OUT_OF_RESOURCES  - cannot allocate protocol data structure
  EFI_DEVICE_ERROR      - cannot create the timer service

--*/
// TODO:    SystemTable - add argument and description to function comment
{
  EFI_STATUS                Status;
  EFI_ACPI_DESCRIPTION      *AcpiDescription;

  EfiInitializeRuntimeDriverLib (ImageHandle, SystemTable, NULL);

  //
  // Initialize AcpiDescription
  //
  AcpiDescription = GetAcpiDescription (&mAcpiDescription);
  if (AcpiDescription == NULL) {
    return EFI_UNSUPPORTED;
  }

  //
  // Make sure the Reset Architectural Protocol is not already installed in the system
  //
  ASSERT_PROTOCOL_ALREADY_INSTALLED (NULL, &gEfiResetArchProtocolGuid);

  //
  // Hook the runtime service table
  //
  SystemTable->RuntimeServices->ResetSystem = EfiAcpiResetSystem;

  //
  // Now install the Reset RT AP on a new handle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mResetHandle,
                  &gEfiResetArchProtocolGuid,
                  NULL,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
