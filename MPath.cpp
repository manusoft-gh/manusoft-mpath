// Copyright 2002 ManuSoft
// https://www.manusoft.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "MPath.h"

#include <sys\stat.h>
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include <io.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

extern "C" IMAGE_DOS_HEADER __ImageBase;


const CHAR WILD_ONE_A             = '?';
const CHAR WILD_ANY_A             = '*';
LPCSTR const WILD_SET_A           = "?*";
const CHAR DRIVE_DELIMITER_A      = ':';
const CHAR DIRECTORY_DELIMITER_A  = '\\';
const CHAR EXTENSION_DELIMITER_A  = '.';
const WCHAR WILD_ONE_W            = L'?';
const WCHAR WILD_ANY_W            = L'*';
LPCWSTR const WILD_SET_W          = L"?*";
const WCHAR DRIVE_DELIMITER_W     = L':';
const WCHAR DIRECTORY_DELIMITER_W = L'\\';
const WCHAR EXTENSION_DELIMITER_W = L'.';


LPCSTR const DLL_EXTENSION_A = "dll";
LPCSTR const INI_EXTENSION_A = "ini";
LPCSTR const EXE_EXTENSION_A = "exe";
LPCSTR const WILD_NAME_EXTENSION_A = "*.*";
LPCWSTR const DLL_EXTENSION_W = L"dll";
LPCWSTR const INI_EXTENSION_W = L"ini";
LPCWSTR const EXE_EXTENSION_W = L"exe";
LPCWSTR const WILD_NAME_EXTENSION_W = L"*.*";


#ifdef UNICODE
#define WILD_ONE             WILD_ONE_W
#define WILD_ANY             WILD_ANY_W
#define WILD_SET             WILD_SET_W
#define DRIVE_DELIMITER      DRIVE_DELIMITER_W
#define DIRECTORY_DELIMITER  DIRECTORY_DELIMITER_W
#define EXTENSION_DELIMITER  EXTENSION_DELIMITER_W
#define DLL_EXTENSION        DLL_EXTENSION_W
#define INI_EXTENSION        INI_EXTENSION_W
#define EXE_EXTENSION        EXE_EXTENSION_W
#define WILD_NAME_EXTENSION  WILD_NAME_EXTENSION_W
#else
#define WILD_ONE             WILD_ONE_A
#define WILD_ANY             WILD_ANY_A
#define WILD_SET             WILD_SET_A
#define DRIVE_DELIMITER      DRIVE_DELIMITER_A
#define DIRECTORY_DELIMITER  DIRECTORY_DELIMITER_A
#define EXTENSION_DELIMITER  EXTENSION_DELIMITER_A
#define DLL_EXTENSION        DLL_EXTENSION_A
#define INI_EXTENSION        INI_EXTENSION_A
#define EXE_EXTENSION        EXE_EXTENSION_A
#define WILD_NAME_EXTENSION  WILD_NAME_EXTENSION_A
#endif


inline LPCSTR AS_CSTR( LPCSTR psz ) { return psz; }
inline LPCSTR AS_CSTR( LPCWSTR psz )
{
	static CHAR sz[MAX_PATH + 1];
	WideCharToMultiByte( CP_ACP, WC_SEPCHARS, psz, -1, sz, MAX_PATH + 1, NULL, NULL );
	return sz;
}
inline LPCWSTR AS_CWSTR( LPCSTR psz )
{
	static WCHAR sz[MAX_PATH + 1];
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, psz, -1, sz, MAX_PATH + 1 );
	return sz;
}
inline LPCWSTR AS_CWSTR( LPCWSTR psz ) { return psz; }

#ifdef UNICODE
inline LPCTSTR AS_TSTR( LPCSTR psz ) { return AS_CWSTR( psz ); }
inline LPCTSTR AS_TSTR( LPCWSTR psz ) { return AS_CWSTR( psz ); }
#else
inline LPCTSTR AS_TSTR( LPCSTR psz ) { return AS_CSTR( psz ); }
inline LPCTSTR AS_TSTR( LPCWSTR psz ) { return AS_CSTR( psz ); }
#endif


void SetLength( RString& sText, int ccLength )
{
	sText.GetBufferSetLength( ccLength );
	sText.ReleaseBuffer();
}	


void StripLeadingChar( RString& sText, TCHAR chLeading )
{
	int ccLength = sText.GetLength();
	
	if( ccLength == 0 )
		return;

	if( sText[0] == chLeading )
		sText = sText.Right( ccLength - 1 );
}


void StripLeadingBackslash( RString& sDirectory )
{
	int ccLength = sDirectory.GetLength ();
	
	if( ccLength <= 1 )
		return;

	if( sDirectory[0] == DIRECTORY_DELIMITER )
		sDirectory = sDirectory.Right( ccLength - 1 );
}
		
	
void StripTrailingChar( RString& sText, TCHAR chTrailing )
{
	int	ccLength = sText.GetLength();
	
	if( ccLength == 0 )
		return;
	
	if( sText[ccLength - 1] == chTrailing )
		SetLength( sText, ccLength - 1 );	
}


void StripTrailingBackslash( RString& sDirectory )
{
	int	ccLength = sDirectory.GetLength();

	if( ccLength <= 1 )
		return;
		
	if( sDirectory[ccLength - 1] == DIRECTORY_DELIMITER )
		SetLength( sDirectory, ccLength - 1 );
}


void EnsureTrailingBackslash( RString& sDirectory )
{
	int	ccLength = sDirectory.GetLength();

	if( sDirectory.IsEmpty() || sDirectory[ccLength - 1] != DIRECTORY_DELIMITER )
		sDirectory = sDirectory + DIRECTORY_DELIMITER;
}
	

void EnsureLeadingBackslash( RString& sDirectory )
{
	if( sDirectory.IsEmpty() || sDirectory[0] != DIRECTORY_DELIMITER )
		sDirectory = DIRECTORY_DELIMITER + sDirectory;
}


#ifdef MPATH_EXTENDED
RString RandomDigits( int nDigits )
{
	ASSERT( nDigits < 20 );
	RString sDigits;
	_stprintf( sDigits.GetBuffer( 512 ), _T("%.4x%.4x%.4x%.4x%.4x"), rand (), rand (), rand (), rand (), rand () );
  sDigits.ReleaseBuffer();
	return sDigits.Left( nDigits );
}
#endif



inline void MPath::Init()
{
#ifdef MPATH_EXTENDED
	m_dwFindFileAttributes = 0;
	m_hFindFile = NULL;
#endif
}


inline void MPath::Exit()
{
#ifdef MPATH_EXTENDED
	if( m_hFindFile != NULL )
		FindClose( m_hFindFile );
#endif
}


MPath::MPath()
{
	Init();
}


MPath::MPath( const MPath& pPath )
{
	Init();
	m_Path = pPath.m_Path;
}

	
MPath::MPath( LPCSTR lpszPath )
{
	Init();
	m_Path = AS_TSTR( lpszPath );
}

	
MPath::MPath( LPCWSTR lpszPath )
{
	Init();
	m_Path = AS_TSTR( lpszPath );
}

    
#ifdef MPATH_EXTENDED
MPath::MPath( SpecialDirectory eInitialDir )
{
	Init();
	switch( eInitialDir )
	{		 
		case CURRENT_DIRECTORY:
				CurrentDirectory();
				break;

		case WINDOWS_DIRECTORY:
				WindowsDirectory();
				break;
      
		case SYSTEM_DIRECTORY:
				SystemDirectory();
				break;
                  
		case MODULE_DIRECTORY:
				ModuleDirectory();
				break;

		case TEMP_DIRECTORY:
			TempDirectory();
			break;
        
		default:
			ASSERT (FALSE);
			break;
	}
}
#endif // MPATH_EXTENDED


MPath::~MPath ()
{
	Exit();
}


BOOL MPath::operator==( const MPath& pPath ) const
{
	RString sTestPath;
	RString sThisPath;
	
	GetFullyQualified( sThisPath );
	pPath.GetFullyQualified( sTestPath );
	
	return (sThisPath.CompareNoCase( sTestPath ) == 0);
}


MPath& MPath::operator=( const MPath& pPath )
{                   
	if( this == &pPath )
		return *this;

	m_Path = pPath.m_Path;
	return *this;
}



MPath& MPath::operator=( LPCSTR lpszPath )
{
	m_Path = AS_TSTR( lpszPath );
	return *this;
}


MPath& MPath::operator=( LPCWSTR lpszPath )
{
	m_Path = AS_TSTR( lpszPath );
	return *this;
}


MPath::operator LPCSTR() const
{
	return AS_CSTR( (LPCTSTR)m_Path );
}


MPath::operator LPCWSTR() const
{
	return AS_CWSTR( (LPCTSTR)m_Path );
}


void MPath::GetComponents( RString* psDrive, 
													 RString* psDirectory, 
													 RString* psName, 
													 RString* psExtension ) const
{
	_tsplitpath( m_Path, 
							 psDrive? psDrive->GetBuffer( MAX_PATH + 1 ) : NULL,
							 psDirectory? psDirectory->GetBuffer( MAX_PATH + 1 ) : NULL,
							 psName? psName->GetBuffer( MAX_PATH + 1 ) : NULL,
							 psExtension? psExtension->GetBuffer( MAX_PATH + 1 ) : NULL );
                
	if(psDrive)
		psDrive->ReleaseBuffer();            
	if(psDirectory)
		psDirectory->ReleaseBuffer();            
	if(psName)
		psName->ReleaseBuffer();            
	if(psExtension)
		psExtension->ReleaseBuffer(); 


	if( psDrive )
		StripTrailingChar( *psDrive, DRIVE_DELIMITER );
	if( psDirectory )
		StripTrailingBackslash( *psDirectory );
	if( psExtension )
		StripLeadingChar( *psExtension, EXTENSION_DELIMITER );	
}

                       
void MPath::GetDrive( RString& sDrive ) const
{
	GetComponents( &sDrive, NULL, NULL, NULL );
}


void MPath::GetDriveDirectory( RString& sDriveDirectory ) const
{
	RString sDrive;
	RString sDirectory;

	GetComponents( &sDrive, &sDirectory );
	sDriveDirectory = sDrive;
	if( !sDrive.IsEmpty() )
		sDriveDirectory += (DRIVE_DELIMITER + sDirectory);
}


void MPath::GetDirectory( RString& sDirectory ) const
{
	GetComponents( NULL, &sDirectory, NULL, NULL );
}    


void MPath::GetNameExtension( RString& sNameExtension ) const
{
	RString sName;
	RString sExtension;

	GetComponents( NULL, NULL, &sName, &sExtension );
	sNameExtension = sName;
	if( !sExtension.IsEmpty() )
		sNameExtension += (EXTENSION_DELIMITER + sExtension);
}


void MPath::GetName( RString& sName ) const
{
	GetComponents( NULL, NULL, &sName, NULL );
}


void MPath::GetExtension( RString& sExtension ) const
{
	GetComponents( NULL, NULL, NULL, &sExtension );
}   


void MPath::GetFullyQualified( RString& sFullyQualified ) const
{
	_tfullpath( sFullyQualified.GetBuffer( MAX_PATH ), m_Path, MAX_PATH + 1 );
		sFullyQualified.ReleaseBuffer();
}


BOOL MPath::IsValid() const
{
	OFSTRUCT ofTest;
	return (OpenFile( AS_CSTR( m_Path ), &ofTest, OF_PARSE ) != HFILE_ERROR);
}

    
BOOL MPath::IsWild() const
{
	return (m_Path.FindOneOf( WILD_SET ) != -1);
}


MPath& MPath::SetComponents( LPCSTR lpszDrive,
														 LPCSTR lpszDirectory,
														 LPCSTR lpszName,
														 LPCSTR lpszExtension )
{
	_tmakepath( m_Path.GetBuffer( MAX_PATH ),
							AS_TSTR( lpszDrive ),
							AS_TSTR( lpszDirectory ),
							AS_TSTR( lpszName ),
							AS_TSTR( lpszExtension ) );
	m_Path.ReleaseBuffer();
	return *this;
}


MPath& MPath::SetComponents( LPCWSTR lpszDrive,
														 LPCWSTR lpszDirectory,
														 LPCWSTR lpszName,
														 LPCWSTR lpszExtension )
{
	_tmakepath( m_Path.GetBuffer( MAX_PATH ),
							AS_TSTR( lpszDrive ),
							AS_TSTR( lpszDirectory ),
							AS_TSTR( lpszName ),
							AS_TSTR( lpszExtension ) );
	m_Path.ReleaseBuffer();
	return *this;
}

						   
MPath& MPath::SetDrive( CHAR chDrive )
{
	CHAR szDrive[2] = { chDrive, '\0' };
	RString sDrive = AS_TSTR( szDrive );
	RString sDirectory;
	RString sName;
	RString sExtension;
	
	GetComponents( NULL, &sDirectory, &sName, &sExtension );
	SetComponents( sDrive, sDirectory, sName, sExtension );
	return *this;
}

						   
MPath& MPath::SetDrive( WCHAR chDrive )
{
	WCHAR szDrive[2] = { chDrive, L'\0' };
	RString sDrive = AS_TSTR( szDrive );
	RString sDirectory;
	RString sName;
	RString sExtension;
	
	GetComponents( NULL, &sDirectory, &sName, &sExtension );
	SetComponents( sDrive, sDirectory, sName, sExtension );
	return *this;
}


MPath& MPath::SetDirectory( LPCSTR lpszDirectory, BOOL bEnsureAbsolute /*= FALSE*/ )
{
	RString sDrive;
	RString sDirectory = AS_TSTR( lpszDirectory );
	RString sName;
	RString sExtension;
	
	if( bEnsureAbsolute )
		EnsureLeadingBackslash( sDirectory );
	EnsureTrailingBackslash( sDirectory );		

	GetComponents( &sDrive, NULL, &sName, &sExtension );
	SetComponents( sDrive, sDirectory, sName, sExtension );
	return *this;
}


MPath& MPath::SetDirectory( LPCWSTR lpszDirectory, BOOL bEnsureAbsolute /*= FALSE*/ )
{
	RString sDrive;
	RString sDirectory = AS_TSTR( lpszDirectory );
	RString sName;
	RString sExtension;
	
	if( bEnsureAbsolute )
		EnsureLeadingBackslash( sDirectory );
	EnsureTrailingBackslash( sDirectory );		

	GetComponents( &sDrive, NULL, &sName, &sExtension );
	SetComponents( sDrive, sDirectory, sName, sExtension );
	return *this;
}


MPath& MPath::SetDriveDirectory( LPCSTR lpszDriveDirectory )
{
	RString sDriveDirectory = AS_TSTR( lpszDriveDirectory );
	RString sName;
	RString sExtension;
	
	EnsureTrailingBackslash( sDriveDirectory );
	
	GetComponents( NULL, NULL, &sName, &sExtension );
	SetComponents( NULL, sDriveDirectory, sName, sExtension );
	return *this;
}


MPath& MPath::SetDriveDirectory( LPCWSTR lpszDriveDirectory )
{
	RString sDriveDirectory = AS_TSTR( lpszDriveDirectory );
	RString sName;
	RString sExtension;
	
	EnsureTrailingBackslash( sDriveDirectory );
	
	GetComponents( NULL, NULL, &sName, &sExtension );
	SetComponents( NULL, sDriveDirectory, sName, sExtension );
	return *this;
}


MPath& MPath::SetName( LPCSTR lpszName )
{
	RString sDrive;
	RString sDirectory;
	RString sExtension;
	
	GetComponents( &sDrive, &sDirectory, NULL, &sExtension );
	SetComponents( sDrive, sDirectory, AS_TSTR( lpszName ), sExtension );
	return *this;
}


MPath& MPath::SetName( LPCWSTR lpszName )
{
	RString sDrive;
	RString sDirectory;
	RString sExtension;
	
	GetComponents( &sDrive, &sDirectory, NULL, &sExtension );
	SetComponents( sDrive, sDirectory, AS_TSTR( lpszName ), sExtension );
	return *this;
}

    
MPath& MPath::SetExtension( LPCSTR lpszExtension )
{
	RString sDrive;
	RString sDirectory;
	RString sName;
	
	GetComponents( &sDrive, &sDirectory, &sName, NULL );
	SetComponents( sDrive, sDirectory, sName, AS_TSTR( lpszExtension ) );
	return *this;
}

    
MPath& MPath::SetExtension( LPCWSTR lpszExtension )
{
	RString sDrive;
	RString sDirectory;
	RString sName;
	
	GetComponents( &sDrive, &sDirectory, &sName, NULL );
	SetComponents( sDrive, sDirectory, sName, AS_TSTR( lpszExtension ) );
	return *this;
}


MPath& MPath::SetNameExtension( LPCSTR lpszNameExtension )
{
	RString sDrive;
	RString sDirectory;

	GetComponents( &sDrive, &sDirectory, NULL, NULL );
	SetComponents( sDrive, sDirectory, AS_TSTR( lpszNameExtension ), NULL );	
	return *this;
}


MPath& MPath::SetNameExtension( LPCWSTR lpszNameExtension )
{
	RString sDrive;
	RString sDirectory;

	GetComponents( &sDrive, &sDirectory, NULL, NULL );
	SetComponents( sDrive, sDirectory, AS_TSTR( lpszNameExtension ), NULL );	
	return *this;
}


MPath& MPath::AppendDirectory( LPCSTR lpszSubDirectory )
{                                               
	RString sDrive;
	RString sDirectory;
	RString sSubDirectory = AS_TSTR( lpszSubDirectory );
	RString sName;
	RString sExtension;
	
	if( sSubDirectory.IsEmpty() )
		return *this;

	StripLeadingBackslash( sSubDirectory );
	EnsureTrailingBackslash( sSubDirectory );

	GetComponents( &sDrive, &sDirectory, &sName, &sExtension );
	EnsureTrailingBackslash( sDirectory );
	SetComponents( sDrive, sDirectory + sSubDirectory, sName, sExtension );
	return *this;
}


MPath& MPath::AppendDirectory( LPCWSTR lpszSubDirectory )
{                                               
	RString sDrive;
	RString sDirectory;
	RString sSubDirectory = AS_TSTR( lpszSubDirectory );
	RString sName;
	RString sExtension;
	
	if( sSubDirectory.IsEmpty() )
		return *this;

	StripLeadingBackslash( sSubDirectory );
	EnsureTrailingBackslash( sSubDirectory );

	GetComponents( &sDrive, &sDirectory, &sName, &sExtension );
	EnsureTrailingBackslash( sDirectory );
	SetComponents( sDrive, sDirectory + sSubDirectory, sName, sExtension );
	return *this;
}


MPath& MPath::UpDirectory( RString* psLastDirectory /*= NULL*/ )
{
	RString sDirectory;
	int nDelimiter;

	GetDirectory( sDirectory );	
	StripTrailingBackslash( sDirectory );
	if( sDirectory.IsEmpty() )
		return *this;
	
	nDelimiter = sDirectory.ReverseFind( DIRECTORY_DELIMITER );
	
	if( psLastDirectory != NULL )
	{
		*psLastDirectory = sDirectory.Mid( nDelimiter );
		StripLeadingBackslash( *psLastDirectory );
	}
		
	if( nDelimiter >= 0 )
		sDirectory = sDirectory.Left( nDelimiter );
		
	SetDirectory( sDirectory );
	return *this;
}

		
MPath& MPath::Empty()
{
	m_Path.Empty();
	return *this;
}


MPath& MPath::MakeRoot()
{
	SetDirectory( (LPCTSTR)NULL );
	SetNameExtension( (LPCTSTR)NULL );
	return *this;
}


BOOL MPath::DirectoryExists() const
{
	return (~(DWORD)0 != ::GetFileAttributes( MPath( *this ).SetNameExtension( (LPCTSTR)NULL ) ));
}                                                     


BOOL MPath::Exists() const
{
	WIN32_FIND_DATA FindData;
	HANDLE hSearch = FindFirstFile( m_Path,&FindData );
	BOOL bSuccess = (hSearch != INVALID_HANDLE_VALUE);
	FindClose( hSearch );
	return bSuccess;
}

#ifdef MPATH_EXTENDED
MPath& MPath::CurrentDirectory()
{
	RString sDriveDirectory;
	
	_tgetcwd( sDriveDirectory.GetBuffer( MAX_PATH ), MAX_PATH );
	sDriveDirectory.ReleaseBuffer();
	
	Empty();
	SetDriveDirectory( sDriveDirectory );
	return *this;
}


MPath& MPath::WindowsDirectory()
{
	RString sDriveDirectory;
	
	GetWindowsDirectory( sDriveDirectory.GetBuffer( MAX_PATH ), MAX_PATH );
	sDriveDirectory.ReleaseBuffer();

	Empty();
	SetDriveDirectory( sDriveDirectory );
	return *this;
}


MPath& MPath::SystemDirectory()
{
	RString sDriveDirectory;
	
	GetSystemDirectory( sDriveDirectory.GetBuffer( MAX_PATH ), MAX_PATH );
	sDriveDirectory.ReleaseBuffer();

	Empty();
	SetDriveDirectory( sDriveDirectory );
	return *this;
}


MPath& MPath::ModulePath( HMODULE hmodTarget /*= NULL*/ )
{
	GetModuleFileName( hmodTarget? hmodTarget : (HMODULE)&__ImageBase,
										 m_Path.GetBuffer( MAX_PATH ),
										 MAX_PATH );
	m_Path.ReleaseBuffer();                   
	return *this;
}


MPath& MPath::ModuleDirectory()
{
	ModulePath();
	SetNameExtension( (LPCTSTR)NULL );
	return *this;
}


MPath& MPath::ModuleDirectory( HMODULE hmodTarget )
{
	ModulePath();
	SetNameExtension( (LPCTSTR)NULL );
	return *this;
}


MPath& MPath::TempDirectory()
{
	::GetTempPath( MAX_PATH, m_Path.GetBuffer( MAX_PATH ) );
	m_Path.ReleaseBuffer();
	SetNameExtension( (LPCTSTR)NULL );
	return *this;
}
        

MPath& MPath::LocalProfile( LPCSTR lpszName, LPCSTR lpszExtension /*= NULL*/ )
{
	ModuleDirectory();
    
	if( lpszExtension == NULL )
		lpszExtension = INI_EXTENSION_A;

	SetName( lpszName );
	SetExtension( lpszExtension );	        
	return *this;
}
        

MPath& MPath::LocalProfile( LPCWSTR lpszName, LPCWSTR lpszExtension /*= NULL*/ )
{
	ModuleDirectory();
    
	if( lpszExtension == NULL )
		lpszExtension = INI_EXTENSION_W;

	SetName( lpszName );
	SetExtension( lpszExtension );	        
	return *this;
}

            
MPath& MPath::LocalProfile( UINT nNameResourceID, UINT nExtensionResourceID /*= 0*/ )
{
	RString sName;
	RString sExtension;

	if( nExtensionResourceID )
		sExtension.LoadString( nExtensionResourceID );
	else
		sExtension = INI_EXTENSION;
          
	if( sName.LoadString( nNameResourceID ) )
		LocalProfile( sName, sExtension );
	return *this;
}


MPath& MPath::PrivateProfile()
{
	MPath sWindowsDirectory( WINDOWS_DIRECTORY );
    
	ModulePath();
	SetDriveDirectory( (LPCTSTR)sWindowsDirectory );
	SetExtension( INI_EXTENSION );
	return *this;
}

    
BOOL MPath::CreateRandomName( BOOL bMustNotExist /*= TRUE*/, UINT nRetries /*= 1000*/ )
{
	UINT nRetry;
	RString sName; 

	for( nRetry = 0; nRetry < nRetries; nRetry++ )
	{
		sName = RandomDigits( MAX_PATH );
		SetName( sName );
		if( !Exists() || !bMustNotExist )
			return TRUE;
	}
	return FALSE;
}


BOOL MPath::CreateSimilarName( BOOL bMustNotExist /*= TRUE*/, UINT nRetries /*= 1000*/ )
{
	UINT nRetry;
	RString sNewName;
	RString sOriginalName;

	GetName( sOriginalName );
		
	for( nRetry = 0; nRetry < nRetries; nRetry++ )
	{
		int ctDigits = 8 - sOriginalName.GetLength();
		if( ctDigits <= 0 )
			ctDigits = 8;
		sNewName = sOriginalName + RandomDigits( ctDigits );
		SetName( sNewName );
		if( !Exists() || !bMustNotExist )
			return TRUE;
	}
	return FALSE;
}


BOOL MPath::Delete( BOOL bEvenIfReadOnly )
{
	if( bEvenIfReadOnly )
	{
		if( _taccess( m_Path, _S_IWRITE ) != -1 ) //does the file have write access?
		{
			if( _tchmod( m_Path, _S_IWRITE ) == -1 )
				TRACE( _T("ERROR - MPath::Delete was unable to delete file") );
		}
	}
	return !_tremove( (LPCTSTR)*this );
}	


BOOL MPath::Rename( LPCSTR lpszNewPath )
{
	return !_trename( (LPCTSTR)*this, AS_TSTR( lpszNewPath ) );
}	


BOOL MPath::Rename( LPCWSTR lpszNewPath )
{
	return !_trename( (LPCTSTR)*this, AS_TSTR( lpszNewPath ) );
}	


BOOL MPath::IsDirectoryEmpty() const
{
	MPath	FileSpec = *this;
	
	FileSpec.SetNameExtension( WILD_NAME_EXTENSION );
	return (!FileSpec.FindFirst());
}

	
BOOL MPath::IsRemovableDrive() const
{
	return (GetDriveType() == DRIVE_REMOVABLE);
}


BOOL MPath::IsNetworkDrive() const
{
	return (GetDriveType() == DRIVE_REMOTE);
}


BOOL MPath::IsCDRomDrive() const
{
	return (GetDriveType() == DRIVE_CDROM);
}


BOOL MPath::IsRAMDrive() const
{
	return (GetDriveType() == DRIVE_RAMDISK);
}


BOOL MPath::DriveExists() const
{
	MPath Test = *this;
	Test.MakeRoot();
	return Test.DirectoryExists();
}


LONG MPath::DriveFreeSpaceBytes() const
{
	DWORD dwSectorsPerCluster;
	DWORD dwBytesPerSector;
	DWORD dwFreeClusters;
	DWORD dwClusters;

	if( !GetDiskFreeSpace( &dwSectorsPerCluster,
												 &dwBytesPerSector,
												 &dwFreeClusters,
												 &dwClusters ) )
		return 0;
	else
		return dwFreeClusters * dwSectorsPerCluster * dwBytesPerSector;
}


LONG MPath::DriveTotalSpaceBytes() const
{
	DWORD dwSectorsPerCluster;
	DWORD dwBytesPerSector;
	DWORD dwFreeClusters;
	DWORD dwClusters;

	if( !GetDiskFreeSpace( &dwSectorsPerCluster,
												 &dwBytesPerSector,
												 &dwFreeClusters,
												 &dwClusters ) )
		return 0;
	else
		return dwClusters * dwSectorsPerCluster * dwBytesPerSector;
}


LONG MPath::GetDriveClusterSize() const
{
	DWORD dwSectorsPerCluster;
	DWORD dwBytesPerSector;
	DWORD dwFreeClusters;
	DWORD dwClusters;

	if( !GetDiskFreeSpace( &dwSectorsPerCluster,
												 &dwBytesPerSector,
												 &dwFreeClusters,
												 &dwClusters ) )
		return 0;
	else
		return dwSectorsPerCluster * dwBytesPerSector;
}


BOOL AttributesMatch(DWORD dwTargetAttributes, DWORD dwFileAttributes)
{
	if( dwTargetAttributes == _A_NORMAL )
		return (0 == (_A_SUBDIR & dwFileAttributes));
	else
		return (0 != (dwTargetAttributes & dwFileAttributes) &&
						((0 != (_A_SUBDIR & dwTargetAttributes)) == 
							(0 != (_A_SUBDIR & dwFileAttributes))));
}


BOOL MPath::FindFirst( DWORD dwAttributes /*= _A_NORMAL*/ )
{
	m_dwFindFileAttributes = dwAttributes;
	BOOL bGotFile;
	BOOL bWantSubdirectory = (0 != (_A_SUBDIR & dwAttributes));

	WIN32_FIND_DATA FindData;
	m_hFindFile = FindFirstFile( m_Path, &FindData );
	
	bGotFile = (m_hFindFile != INVALID_HANDLE_VALUE);
	while( bGotFile )
	{
		if( !AttributesMatch( m_dwFindFileAttributes, FindData.dwFileAttributes ))
			goto getnextfile;

		if( bWantSubdirectory && (FindData.cFileName[0] == '.') )
			goto getnextfile;

		if( 0 != (_A_SUBDIR & m_dwFindFileAttributes) )
			StripTrailingBackslash( m_Path );
		SetNameExtension( FindData.cFileName );
	    if( 0 != (_A_SUBDIR & dwAttributes) )
    	   	EnsureTrailingBackslash( m_Path );
		return TRUE;
	
	getnextfile:
		bGotFile = FindNextFile( m_hFindFile, &FindData );
	}
	
	return FALSE;
}


BOOL MPath::FindNext()
{
	WIN32_FIND_DATA FindData;

	ASSERT( m_hFindFile != NULL );
	while( FindNextFile( m_hFindFile, &FindData ) != FALSE )
	{
		if( AttributesMatch( m_dwFindFileAttributes, FindData.dwFileAttributes ) )
		{
			if( 0 != (_A_SUBDIR & m_dwFindFileAttributes) )
			{
				UpDirectory();
				AppendDirectory( FindData.cFileName );
			}
			else
				SetNameExtension( FindData.cFileName );
			return TRUE;
		}
	}
	return FALSE;
}


LONG MPath::GetSize() const
{
	struct _tstat stStat;
	int nResult;
	nResult = _tstat( m_Path, &stStat );

	return (nResult == 0)? stStat.st_size : 0;
}

				
CTime MPath::GetTime() const
{
	struct _tstat stStat;
	int nResult;
	nResult = _tstat( m_Path, &stStat );

	return (nResult == 0)? stStat.st_mtime : 0;
}


BOOL MPath::ChangeDirectory()
{
	RString	sDriveDirectory;
	GetDriveDirectory( sDriveDirectory );
	return (_tchdir( sDriveDirectory ) == 0);
}


BOOL MPath::RemoveDirectory()
{
	RString sDriveDirectory;
	GetDriveDirectory( sDriveDirectory );
	return (_trmdir( sDriveDirectory ) ==  0);
}

	
BOOL MPath::CreateDirectory( BOOL bCreateIntermediates /*= TRUE*/ )
{
	RString sPathText( m_Path );
	BOOL bSuccess;
		
	StripTrailingBackslash( sPathText );
	bSuccess = (_tmkdir( sPathText ) ==  0);
	if( !bSuccess )
		bSuccess = ChangeDirectory();
		
	if( !bSuccess && bCreateIntermediates )
	{
		int	nDelimiter = sPathText.ReverseFind( DIRECTORY_DELIMITER );
		if( nDelimiter == -1 )
			return FALSE;

		SetLength( sPathText, nDelimiter );
		MPath	pathSubdir = sPathText;
		
		if( pathSubdir.CreateDirectory() )
			return CreateDirectory( FALSE );
		else
			return FALSE;
	}
	return bSuccess;
}


BOOL MPath::GetDiskFreeSpace( LPDWORD lpSectorsPerCluster,
															LPDWORD lpBytesPerSector,
															LPDWORD lpFreeClusters,
															LPDWORD lpClusters ) const
{
	MPath	pathRoot = *this;

	pathRoot.MakeRoot();
	return ::GetDiskFreeSpace( pathRoot, 
														 lpSectorsPerCluster,
														 lpBytesPerSector,
														 lpFreeClusters,
														 lpClusters );
}


UINT MPath::GetDriveType() const
{
	MPath	pathRoot = *this;

	pathRoot.MakeRoot();
	return ::GetDriveType( pathRoot );
}
#endif //MPATH_EXTENDED
