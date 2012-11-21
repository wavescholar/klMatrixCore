#include "SecurityPermission.h"

//This is for revokation with CertificateServices.
//#include "Certadm.h"
//#include "Wincrypt.h"
//		void revoke()
//		{
//			BSTR bstrCA = NULL;
//    BSTR bstrSerial = NULL;  // certificate serial number
//    long nReason;
//    DATE RevokeDate;         // revocation date
//    SYSTEMTIME st;
//
//    bstrSerial = SysAllocString(L"<SERIALNUMBERHERE>");
//    bstrCA = SysAllocString(L"<COMPUTERNAMEHERE>\\<CANAMEHERE>");
//    if (NULL == bstrCA || NULL == bstrSerial)
//    {
//        //printf("Memory allocation failed\n");
//        goto error;
//    }
//    
//    nReason = CRL_REASON_AFFILIATION_CHANGED;  // Defined
//	                                      // in Wincrypt.h
//
//    //  Specify when the cert should be revoked.
//    //  Note: To revoke immediately, set RevokeDate to zero.
//    //  This example sets the revoke date to noon on 1/1/2001.
//    //  Zero out values first (avoids setting minutes, seconds,
//    //  and so on).
//    memset(&st, 0, sizeof(SYSTEMTIME));
//    st.wYear = 2001;
//    st.wMonth = 1;     // Jan
//    st.wDay = 1;       // 1st day of month
//    st.wHour = 12;     // Noon
//
//    //  Place the date in the required format.
//    if (!SystemTimeToVariantTime(&st, &RevokeDate))
//    {
//        //printf("Unable to convert time.\n");
//        goto error;
//    }
//
//    //  Revoke the certificate.
//    //  pCertAdmin is a previously instantiated ICertAdmin object.
//	HRESULT     hr = ICertAdmin2::RevokeCertificate( bstrCA,
//                                        bstrSerial,
//                                        nReason,
//                                        RevokeDate );
//    if (FAILED(hr))
//    {
//       // printf("Failed RevokeCertificate. [%x]\n", hr);
//        goto error;
//    }
//    else
//        //printf("Certificate %ws revoked.\n", bstrSerial );
//
//    //  Done processing.
//
//error:
//
//    //  Free resources.
//    if (bstrSerial)
//        SysFreeString( bstrSerial );
//    if (bstrCA)
//        SysFreeString( bstrCA );
//
//
//
//		}
//
//

HRESULT enumProviders(void);
void enumerates_system_certificate_store_locations(void);
void enumerateCertificatesInStore(const wchar_t* pszStoreNameIn);

using namespace System;
using namespace System::Security;
using namespace System::Security::Permissions;
using namespace System::Runtime::InteropServices;


[assembly:CLSCompliant(true)];
private ref class SecurityGenerator
{
private:
	array<SecurityPermissionFlag>^mySecurity;
	int reflectionIndex;

public:
	SecurityGenerator()
	{
		array<SecurityPermissionFlag>^ temp = {SecurityPermissionFlag::AllFlags, 
			SecurityPermissionFlag::Assertion |
			SecurityPermissionFlag::ControlAppDomain,
			SecurityPermissionFlag::ControlAppDomain,
			SecurityPermissionFlag::ControlDomainPolicy |
			SecurityPermissionFlag::ControlEvidence,
			SecurityPermissionFlag::ControlEvidence,
			SecurityPermissionFlag::ControlPolicy |
			SecurityPermissionFlag::ControlPrincipal,
			SecurityPermissionFlag::ControlPrincipal,
			SecurityPermissionFlag::ControlThread,
			SecurityPermissionFlag::Execution,
			SecurityPermissionFlag::Infrastructure,
			SecurityPermissionFlag::NoFlags,
			SecurityPermissionFlag::RemotingConfiguration |
			SecurityPermissionFlag::SerializationFormatter,
			SecurityPermissionFlag::SerializationFormatter,
			SecurityPermissionFlag::SkipVerification,
			SecurityPermissionFlag::UnmanagedCode};
		mySecurity = temp;
		reflectionIndex = 0;
		ResetIndex();
	}

	void ResetIndex()
	{
		reflectionIndex = 0;
	}
	// GetSecurityFlag creates a SecurityPermission object.
	bool GetSecurityFlag( [Out]SecurityPermissionFlag * security )
	{
		if ( reflectionIndex >= mySecurity->Length )
		{       
			*security = SecurityPermissionFlag::NoFlags;
			reflectionIndex++;
			return false;
		}
		*security = mySecurity[ reflectionIndex++ ];
		return true;
	}
};

// End of SecurityGenerator.
namespace SecurityProvider
{
	//[assembly:AssemblyKeyFileAttribute("C:\Security\WcfServiceLibrary\WcfServiceLibrary\bin\x64\Release\keyfilename.snk")];
	public ref class klSecurityWrapper
	{
	private:

		// IsSubsetOf determines whether the current permission is a subset of the specified permission.
		bool IsSubsetOfDemo()
		{
			bool returnValue = true;
			SecurityPermissionFlag security1;
			SecurityPermissionFlag security2;
			SecurityPermission^ securityPerm1;
			SecurityPermission^ securityPerm2;
			SecurityGenerator^ SecurityGen1 = gcnew SecurityGenerator;
			SecurityGenerator^ SecurityGen2 = gcnew SecurityGenerator;
			SecurityGen1->ResetIndex();
			while ( SecurityGen1->GetSecurityFlag( &security1 ) )
			{
				securityPerm1 = gcnew SecurityPermission(security1);

				SecurityGen2->ResetIndex();
				Console::WriteLine( "********************************************************\n" );
				while ( SecurityGen2->GetSecurityFlag( &security2 ) )
				{
					securityPerm2 = gcnew SecurityPermission(security2);
					try
					{
						if ( securityPerm1->IsSubsetOf( securityPerm2 ) )
						{
							Console::WriteLine( "{0} is a subset of {1}", securityPerm1->Flags, securityPerm2->Flags );
						}
						else
						{
							Console::WriteLine( "{0} is not a subset of {1}", securityPerm1->Flags, securityPerm2->Flags );
						}
					}
					catch ( Exception^ e ) 
					{
						Console::WriteLine( "An exception was thrown for subset :{0}\n{1}\n{2}", securityPerm1, securityPerm2, e );
						returnValue = false;
					}

				}
			}

			return returnValue;
		}


		// Union creates a new permission that is the union of the current permission and the specified permission.
		bool UnionDemo()
		{
			bool returnValue = true;
			SecurityPermissionFlag security1;
			SecurityPermissionFlag security2;
			SecurityPermission^ securityPerm1;
			SecurityPermission^ securityPerm2;
			IPermission^ SecurityPerm3;
			SecurityGenerator^ SecurityGen1 = gcnew SecurityGenerator;
			SecurityGenerator^ SecurityGen2 = gcnew SecurityGenerator;
			SecurityGen1->ResetIndex();
			while ( SecurityGen1->GetSecurityFlag( &security1 ) )
			{
				securityPerm1 = gcnew SecurityPermission(security1);

				Console::WriteLine( "**********************************************************\n" );
				SecurityGen2->ResetIndex();
				while ( SecurityGen2->GetSecurityFlag( &security2 ) )
				{
					securityPerm2 = gcnew SecurityPermission(security2);
					SecurityPerm3 = dynamic_cast<SecurityPermission^>(securityPerm1->Union( securityPerm2 ));
					SecurityPerm3 = securityPerm1->Union( securityPerm2 );
					if ( SecurityPerm3 == nullptr )
					{
						Console::WriteLine( "The union of {0} and {1} is null.", security1, security2 );
					}
					else
					{
						Console::WriteLine( "The union of {0} and {1} = {2}", securityPerm1->Flags, securityPerm2->Flags, (dynamic_cast<SecurityPermission^>(SecurityPerm3))->Flags );
					}
				}
			}

			return returnValue;
		}


		// Intersect creates and returns a new permission that is the intersection of the current 
		// permission and the permission specified.
		bool IntersectDemo()
		{
			bool returnValue = true;
			SecurityPermissionFlag security1;
			SecurityPermissionFlag security2;
			SecurityPermission^ securityPerm1;
			SecurityPermission^ securityPerm2;
			SecurityPermission^ SecurityPerm3;
			SecurityGenerator^ SecurityGen1 = gcnew SecurityGenerator;
			SecurityGenerator^ SecurityGen2 = gcnew SecurityGenerator;
			SecurityGen1->ResetIndex();
			while ( SecurityGen1->GetSecurityFlag(  &security1 ) )
			{
				securityPerm1 = gcnew SecurityPermission(security1);

				Console::WriteLine( "**********************************************************\n" );
				SecurityGen2->ResetIndex();
				while ( SecurityGen2->GetSecurityFlag(  &security2 ) )
				{
					securityPerm2 = gcnew SecurityPermission(security2);
					SecurityPerm3 = dynamic_cast<SecurityPermission^>(securityPerm1->Intersect( securityPerm2 ));
					if ( SecurityPerm3 != nullptr )
					{
						Console::WriteLine( "The intersection of {0} and {1} = {2}", security1, security2, (dynamic_cast<SecurityPermission^>(SecurityPerm3))->Flags );
					}
					else
					{
						Console::WriteLine( "The intersection of {0} and {1} is null.", security1, security2 );
					}
				}
			}

			return returnValue;
		}


		//Copy creates and returns an identical copy of the current permission.
		bool CopyDemo()
		{
			bool returnValue = true;
			SecurityPermissionFlag security1;
			SecurityPermission^ securityPerm1;
			SecurityPermission^ securityPerm2;
			SecurityGenerator^ SecurityGen1 = gcnew SecurityGenerator;
			SecurityGenerator^ SecurityGen2 = gcnew SecurityGenerator;
			SecurityGen1->ResetIndex();
			while ( SecurityGen1->GetSecurityFlag( &security1 ) )
			{
				securityPerm1 = gcnew SecurityPermission(security1);
				continue;

				SecurityGen2->ResetIndex();
				Console::WriteLine( "********************************************************\n" );
				try
				{
					securityPerm2 = dynamic_cast<SecurityPermission^>(securityPerm1->Copy());
					Console::WriteLine( "Result of copy = {0}", securityPerm2 );
				}
				catch ( Exception^ e ) 
				{
					Console::WriteLine( "Copy failed :{0}{1}", securityPerm1, e );
					continue;
				}

			}

			return returnValue;
		}


		// ToXml creates an XML encoding of the permission and its current state; 
		// FromXml reconstructs a permission with the specified state from the XML encoding. 
		bool ToFromXmlDemo()
		{
			bool returnValue = true;
			SecurityPermissionFlag security1;
			SecurityPermission^ securityPerm1;
			SecurityPermission^ securityPerm2;
			SecurityGenerator^ SecurityGen1 = gcnew SecurityGenerator;
			SecurityGenerator^ SecurityGen2 = gcnew SecurityGenerator;
			SecurityGen1->ResetIndex();
			while ( SecurityGen1->GetSecurityFlag( &security1 ) )
			{
				securityPerm1 = gcnew SecurityPermission(security1);
				continue;

				Console::WriteLine( "********************************************************\n" );
				SecurityGen2->ResetIndex();
				try
				{
					securityPerm2 = gcnew SecurityPermission( PermissionState::None );
					securityPerm2->FromXml( securityPerm1->ToXml() );
					Console::WriteLine( "Result of ToFromXml = {0}", securityPerm2 );
				}
				catch ( Exception^ e ) 
				{
					Console::WriteLine( "ToFromXml failed :{0}{1}", securityPerm1, e );
					continue;
				}

			}

			return returnValue;
		}


	public:

		void enumerateSecurityProviders()
		{
			HRESULT ans = enumProviders();
		}

		void enumerateCertificates()
		{
			enumerateCertificatesInStore(L"My");
			enumerateCertificatesInStore(L"CurrentUser");
			enumerateCertificatesInStore(L"LocalMachine");

			enumerateCertificatesInStore(L"CERT_SYSTEM_STORE_CURRENT_USER");
			enumerateCertificatesInStore(L"CERT_SYSTEM_STORE_CURRENT_SERVICE");
			enumerateCertificatesInStore(L"CERT_SYSTEM_STORE_LOCAL_MACHINE");
			enumerateCertificatesInStore(L"CERT_SYSTEM_STORE_LOCAL_MACHINE_GROUP_POLICY");
			enumerateCertificatesInStore(L"CERT_SYSTEM_STORE_CURRENT_USER_GROUP_POLICY");
			enumerateCertificatesInStore(L"CERT_SYSTEM_STORE_SERVICES");
			enumerateCertificatesInStore(L"CERT_SYSTEM_STORE_USERS");
			enumerateCertificatesInStore(L"CERT_SYSTEM_STORE_LOCAL_MACHINE_ENTERPRISE");
		}

		void enumerateCertificateStoreLocations()
		{
			enumerates_system_certificate_store_locations();
		}





		// Invoke all demos.
		bool runDemo()
		{


			bool returnCode = true;
			bool tempReturnCode;

			// Call the IsSubsetOf demo.
			tempReturnCode = IsSubsetOfDemo();
			if ( tempReturnCode )
				Console::Out->WriteLine( "IsSubsetOf demo completed successfully." );
			else
				Console::Out->WriteLine( "IsSubsetOf demo failed." );

			returnCode = tempReturnCode && returnCode;

			// Call the Union demo.
			tempReturnCode = UnionDemo();
			if ( tempReturnCode )
				Console::Out->WriteLine( "Union demo completed successfully." );
			else
				Console::Out->WriteLine( "Union demo failed." );

			returnCode = tempReturnCode && returnCode;

			// Call the Intersect demo. 
			tempReturnCode = IntersectDemo();
			if ( tempReturnCode )
				Console::Out->WriteLine( "Intersect demo completed successfully." );
			else
				Console::Out->WriteLine( "Intersect demo failed." );

			returnCode = tempReturnCode && returnCode;

			// Call the Copy demo. 
			tempReturnCode = CopyDemo();
			if ( tempReturnCode )
				Console::Out->WriteLine( "Copy demo completed successfully." );
			else
				Console::Out->WriteLine( "Copy demo failed." );

			returnCode = tempReturnCode && returnCode;

			// Call the ToFromXml demo. 
			tempReturnCode = ToFromXmlDemo();
			if ( tempReturnCode )
				Console::Out->WriteLine( "ToFromXml demo completed successfully." );
			else
				Console::Out->WriteLine( "ToFromXml demo failed." );

			returnCode = tempReturnCode && returnCode;
			return (returnCode);
		}

	public: 
		void  testSecurityPermissions()
		{
			try
			{
				klSecurityWrapper^ democase = gcnew klSecurityWrapper;
				bool returnCode = democase->runDemo();
				if ( returnCode )
				{
					Console::Out->WriteLine( "SecurityPermission demo completed successfully." );

				}
				else
				{
					Console::Out->WriteLine( "SecurityPermission demo failed." );

				}
			}
			catch ( Exception^ e ) 
			{
				Console::Out->WriteLine( "SecurityPermission demo failed." );
				Console::WriteLine( e );
				Console::Out->WriteLine( "Press the Enter key to exit." );
				String^ consoleInput = Console::ReadLine();
				System::Environment::ExitCode = 101;
			}

		}
	};
};


