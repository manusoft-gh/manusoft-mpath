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

#ifndef _MPATH_H_
#define _MPATH_H_

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#include <afx.h>
#include <afxwin.h>
#include <dos.h>
#ifdef MPATH_NOCSTRING
#include "RString.h"
#else
#define RString CString
#endif


class MPath
{
protected:
	RString m_Path;

#ifdef MPATH_EXTENDED
public:
	enum SpecialDirectory
	{
		CURRENT_DIRECTORY,
		WINDOWS_DIRECTORY,
		SYSTEM_DIRECTORY,
		MODULE_DIRECTORY,
		TEMP_DIRECTORY
	};
#endif

#ifdef MPATH_NOCSTRING
public:
	operator RString&() { return m_Path; }
	operator const RString&() const { return m_Path; }
#endif

public:    
	MPath();
	MPath( const MPath& pPath );
	MPath( LPCSTR lpszPath );
	MPath( LPCWSTR lpszPath );
#ifdef MPATH_EXTENDED
	MPath( SpecialDirectory eInitialDir );
#endif
	virtual ~MPath();

	inline void Init();
	inline void Exit();


	MPath& operator=( const MPath& pPath );
	MPath& operator=( LPCSTR lpszPath );
	MPath& operator=( LPCWSTR lpszPath );
	operator LPCSTR() const;
	operator LPCWSTR() const;
	BOOL operator==( const MPath& rPath ) const;
    

	void GetDrive( RString& sDrive ) const;
	void GetDriveDirectory( RString& sDriveDirectory ) const;
	void GetDirectory( RString& sDirectory ) const;
	void GetName( RString& sName ) const;
	void GetNameExtension( RString& sNameExtension ) const;
	void GetExtension( RString& sExtension ) const;
    
	void GetComponents( RString* psDrive = NULL,
											RString* psDirectory = NULL,
											RString* psName = NULL,
											RString* psExtension = NULL ) const;
	void GetFullyQualified( RString& sFullyQualified ) const;


	BOOL IsEmpty() const { return m_Path.IsEmpty(); }
	BOOL IsValid() const;
	BOOL IsWild() const;
    

	MPath& SetDrive( CHAR chDrive );
	MPath& SetDrive( WCHAR chDrive );
	MPath& SetDriveDirectory( LPCSTR lpszDriveDirectory );
	MPath& SetDriveDirectory( LPCWSTR lpszDriveDirectory );
	MPath& SetDirectory( LPCSTR lpszDirectory, BOOL bEnsureAbsolute = FALSE );
	MPath& SetDirectory( LPCWSTR lpszDirectory, BOOL bEnsureAbsolute = FALSE );
	MPath& SetName( LPCSTR lpszName );
	MPath& SetName( LPCWSTR lpszName );
	MPath& SetNameExtension( LPCSTR lpszNameExtension );
	MPath& SetNameExtension( LPCWSTR lpszNameExtension );
	MPath& SetExtension( LPCSTR lpszExtension );
	MPath& SetExtension( LPCWSTR lpszExtension );

	MPath& AppendDirectory( LPCSTR lpszSubDirectory );
	MPath& AppendDirectory( LPCWSTR lpszSubDirectory );
	MPath& UpDirectory( RString* psLastDirectory = NULL );

	MPath& SetComponents( LPCSTR lpszDrive,
												LPCSTR lpszDirectory,
												LPCSTR lpszName,
												LPCSTR lpszExtension );
	MPath& SetComponents( LPCWSTR lpszDrive,
												LPCWSTR lpszDirectory,
												LPCWSTR lpszName,
												LPCWSTR lpszExtension );

	MPath& MakeRoot();
	MPath& ModulePath( HMODULE hmodTarget = NULL );
	MPath& Empty();

	BOOL Exists() const;
	BOOL DirectoryExists() const;

#ifdef MPATH_EXTENDED
	MPath& CurrentDirectory();
	MPath& WindowsDirectory();
	MPath& SystemDirectory();
	MPath& ModuleDirectory();
	MPath& ModuleDirectory( HMODULE hmodTarget );
	MPath& TempDirectory();

	MPath& PrivateProfile();
	MPath& LocalProfile( UINT nNameResourceID, UINT nExtensionResourceID = 0 );
	MPath& LocalProfile( LPCSTR lpszName, LPCSTR lpszExtension = NULL );
	MPath& LocalProfile( LPCWSTR lpszName, LPCWSTR lpszExtension = NULL );

	BOOL CreateRandomName( BOOL bMustNotExist = TRUE, UINT nRetries = 1000 );	
	BOOL CreateSimilarName( BOOL bMustNotExist = TRUE, UINT nRetries = 1000 );	
		        
	BOOL IsDirectoryEmpty() const;

	BOOL DriveExists() const;
	BOOL IsRemovableDrive() const;
	BOOL IsCDRomDrive() const;
	BOOL IsNetworkDrive() const;
	BOOL IsRAMDrive() const;

	LONG DriveTotalSpaceBytes() const;
	LONG DriveFreeSpaceBytes() const;
	LONG GetDriveClusterSize() const;

	LONG GetSize() const;
	CTime GetTime() const;


	BOOL CreateDirectory( BOOL bCreateIntermediates = TRUE );
	BOOL RemoveDirectory();
	BOOL ChangeDirectory();
	    

	BOOL Delete( BOOL bEvenIfReadOnly = TRUE );
	BOOL Rename( LPCSTR lpszNewPath );
	BOOL Rename( LPCWSTR lpszNewPath );


	BOOL FindFirst( DWORD dwAttributes = _A_NORMAL );
	BOOL FindNext();

	UINT GetDriveType() const;
	BOOL GetDiskFreeSpace( LPDWORD lpSectorsPerCluster,
												 LPDWORD lpBytesPerSector,
												 LPDWORD lpFreeClusters,
												 LPDWORD lpClusters ) const;

protected:
	DWORD m_dwFindFileAttributes;
	HANDLE m_hFindFile;
#endif //MPATH_EXTENDED
};


#endif // !_MPATH_H_
