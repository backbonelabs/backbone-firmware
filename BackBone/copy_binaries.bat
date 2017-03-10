@REM Run from the same directory this file is in

@IF "%1%"=="release" (
    @echo Copy Release Files
    copy BackBone_Bootloadable.cydsn\CortexM0\ARM_GCC_493\Release\BackBone_Bootloadable.cyacd ..\Binaries
    copy BackBone_Bootloadable.cydsn\CortexM0\ARM_GCC_493\Release\BackBone_Bootloadable.hex ..\Binaries
    copy stack.cydsn\CortexM0\ARM_GCC_493\Release\stack.cyacd ..\Binaries
    copy ..\EncryptFirmwareCli\encrypt_cyacd.exe ..\Binaries
    ..\Binaries\encrypt_cyacd.exe  ..\Binaries\BackBone_Bootloadable.cyacd ..\Binaries\BackBone_Bootloadable_Encrypted.cyacd
    ..\Binaries\encrypt_cyacd.exe  ..\Binaries\stack.cyacd ..\Binaries\stack_encrypted.cyacd
) ELSE (
    @echo Copy Debug Files
    copy BackBone_Bootloadable.cydsn\CortexM0\ARM_GCC_493\Debug\BackBone_Bootloadable.cyacd ..\Binaries
    copy BackBone_Bootloadable.cydsn\CortexM0\ARM_GCC_493\Debug\BackBone_Bootloadable.hex ..\Binaries
    copy stack.cydsn\CortexM0\ARM_GCC_493\Debug\stack.cyacd ..\Binaries
    copy ..\EncryptFirmwareCli\encrypt_cyacd.exe ..\Binaries
    ..\Binaries\encrypt_cyacd.exe  ..\Binaries\BackBone_Bootloadable.cyacd ..\Binaries\BackBone_Bootloadable_Encrypted.cyacd
    ..\Binaries\encrypt_cyacd.exe  ..\Binaries\stack.cyacd ..\Binaries\stack_encrypted.cyacd
)
