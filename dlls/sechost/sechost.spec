@ stdcall ChangeServiceConfig2A(long long ptr)
@ stdcall ChangeServiceConfig2W(long long ptr)
@ stdcall ChangeServiceConfigA(long long long long wstr str ptr str str str str)
@ stdcall ChangeServiceConfigW(long long long long wstr wstr ptr wstr wstr wstr wstr)
@ stdcall CloseServiceHandle(long)
@ stdcall ControlService(long long ptr)
@ stdcall CreateServiceA(long str str long long long long str str ptr str str str)
@ stdcall CreateServiceW(long wstr wstr long long long long wstr wstr ptr wstr wstr wstr)
@ stdcall DeleteService(long)
@ stdcall EnumDependentServicesA(long long ptr long ptr ptr)
@ stdcall EnumDependentServicesW(long long ptr long ptr ptr)
@ stdcall EnumServicesStatusA (long long long ptr long ptr ptr ptr)
@ stdcall EnumServicesStatusExA(long long long long ptr long ptr ptr ptr str)
@ stdcall EnumServicesStatusExW(long long long long ptr long ptr ptr ptr wstr)
@ stdcall EnumServicesStatusW (long long long ptr long ptr ptr ptr)
@ stdcall GetServiceDisplayNameA(ptr str ptr ptr)
@ stdcall GetServiceDisplayNameW(ptr wstr ptr ptr)
@ stdcall GetServiceKeyNameA(long str ptr ptr)
@ stdcall GetServiceKeyNameW(long wstr ptr ptr)
@ stdcall I_ScRegisterDeviceNotification(ptr ptr long)
@ stdcall I_ScUnregisterDeviceNotification(ptr)
@ stdcall LockServiceDatabase(ptr)
@ stdcall NotifyServiceStatusChangeW(ptr long ptr)
@ stdcall OpenSCManagerA(str str long)
@ stdcall OpenSCManagerW(wstr wstr long)
@ stdcall OpenServiceA(long str long)
@ stdcall OpenServiceW(long wstr long)
@ stdcall QueryServiceConfig2A(long long ptr long ptr)
@ stdcall QueryServiceConfig2W(long long ptr long ptr)
@ stdcall QueryServiceConfigA(long ptr long ptr)
@ stdcall QueryServiceConfigW(long ptr long ptr)
@ stdcall QueryServiceLockStatusA(long ptr long ptr)
@ stdcall QueryServiceLockStatusW(long ptr long ptr)
@ stdcall QueryServiceObjectSecurity(long long ptr long ptr)
@ stdcall QueryServiceStatusEx (long long ptr long ptr)
@ stdcall QueryServiceStatus(long ptr)
@ stdcall RegisterServiceCtrlHandlerA(str ptr)
@ stdcall RegisterServiceCtrlHandlerExA(str ptr ptr)
@ stdcall RegisterServiceCtrlHandlerExW(wstr ptr ptr)
@ stdcall RegisterServiceCtrlHandlerW(wstr ptr)
@ stdcall SetServiceBits(long long long long)
@ stdcall SetServiceObjectSecurity(long long ptr)
@ stdcall SetServiceStatus(long ptr)
@ stdcall StartServiceA(long long ptr)
@ stdcall StartServiceCtrlDispatcherA(ptr)
@ stdcall StartServiceCtrlDispatcherW(ptr)
@ stdcall StartServiceW(long long ptr)
@ stdcall UnlockServiceDatabase (ptr)
