// -*- c++ -*-
/*
 *  MICO --- an Open Source CORBA implementation
 *  Copyright (c) 1997-2010 by The Mico Team
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  For more information, visit the MICO Home Page at
 *  http://www.mico.org/
 */

#ifndef __corba_h__
#define __corba_h__


/***************************** C headers ****************************/

#include <cassert>
#include <cstdlib>
#include <cstddef> // for wchar_t
#ifndef _POCKET_PC
#include <sys/types.h>
#include <cerrno>
#endif
#include <climits>

/***************************** config *******************************/

// For switching on debug code uncomment line below and comment line under it
//#define MTDEBUG
#undef MTDEBUG

#if defined(_WIN32) && !defined(__MINGW32__)
  #if defined(_MSC_VER)
    #if _MSC_VER < 1300
      #error You need at least VC 7.0 (MSDEV.NET) to compile MICO successfully
    #endif
  #elif defined(_BORLANDC_)
    #error Borland support not implemented yet
    #if _BORLANDC_ < 0x0560
      #error Need (at least) BCB6 Compiler // or whatever is required when the port is ready
    #endif
  #else
    #error unknown (Windows-)Compiler
  #endif
  #include <mico/config-win.h>
#elif defined (_POCKET_PC)
  #include <mico/config-ppc.h>
#else
  #include <mico/config.h>
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


#if (defined(PIC) && defined(HAVE_SHARED_EXCEPTS)) || \
    (!defined(PIC) && defined(HAVE_EXCEPTS))
#define HAVE_EXCEPTIONS 1
#endif

#ifdef MICO_CONF_NO_EXCEPTIONS
#undef HAVE_EXCEPTS
#undef HAVE_SHARED_EXCEPTS
#undef HAVE_EXCEPTIONS
#endif


#ifndef HAVE_NAMESPACE
  #error "MICO must be compiled by a compiler that supports namespaces"
#endif

#if defined (_WIN32) || defined (_POCKET_PC)

#ifdef BUILD_MICO_DLL
#undef  MICO_EXPORT
#define MICO_EXPORT /**/ 
#else // BUILD_MICO_DLL
#undef  MICO_EXPORT
#define MICO_EXPORT __declspec(dllimport)
#endif // BUILD_MICO_DLL

#else // _WIN32 || _POCKET_PC

#undef MICO_EXPORT
#define MICO_EXPORT /**/

#endif // _WIN32 || _POCKET_PC

#ifdef USE_MEMTRACE
// this enables memtracing for STL objects
// The malloc and free macros are undefined under STL includes
#include <mico/memtrace.h>
#endif

/***************************** STL headers **************************/


#ifdef HAVE_MINI_STL
#include <ministl/string>
#include <ministl/vector>
#include <ministl/map>
#include <ministl/set>
#include <ministl/list>

#else

#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <queue>
#include <stack>

#if defined(_WIN32) && !defined(__MINGW32__)
#include <limits>
#endif

#endif


typedef std::vector<int>::size_type mico_vec_size_type;

#ifdef USE_MEMTRACE
#undef malloc
#undef free
#endif
/************************** MICO templates **************************/

#include <mico/version.h>
#include <mico/types.h>
#include <mico/sequence.h>
#include <mico/sequence_special.h>
#include <mico/array.h>
#include <mico/template.h>
#include <mico/fixed.h>
#include <mico/native.h>

// MICOMT needs MICO::Logger from utils
//#include <mico/util.h>

/*************************** module MICOMT **************************/
// we already need to include os-thread even for ST build
#include <mico/os-thread.h>

/*************************** module CORBA ***************************/

#include <mico/basic.h>
#include <mico/magic.h>
#include <mico/address.h>
#include <mico/string.h>
#include <mico/tcconst.h>
#include <mico/ioptypes.h>
#include <mico/ior.h>
#include <mico/transport.h>
#include <mico/buffer.h>
#include <mico/codeset.h>
#include <mico/string.h>
#include <mico/codec.h>
#include <mico/except.h>
#include <mico/orb_excepts.h>
#include <mico/any.h>
#include <mico/object.h>
#include <mico/value.h>
#include <mico/basic_seq.h>
#include <mico/orb_fwd.h>
namespace PortableInterceptor
{
    class ClientRequestInfo;
    class ServerRequestInfo;
    typedef ClientRequestInfo* ClientRequestInfo_ptr;
    typedef ServerRequestInfo* ServerRequestInfo_ptr;
}
namespace PInterceptor
{
    class ClientRequestInfo_impl;
    class ServerRequestInfo_impl;
}
#include <mico/dii.h>
#include <mico/dsi.h>
#include <mico/static.h>

#include <mico/current.h>
#include <mico/policy.h>

#include <mico/timebase.h>

#include <mico/mttypes.h>
#ifdef THREADING_POLICIES
#include <mico/mtpolicy.h>
#endif // THREADING_POLICIES

#ifndef MICO_CONF_NO_POA
namespace PortableServer
{
    class ServantBase;
}

#ifndef MICO_CONF_NO_INTERCEPT
#if (defined(USE_CSL2)) || (defined(USE_CSIV2))
#include <mico/security/csi_base.h>
#endif // USE_CSL2 or USE_CSIV2

#ifdef USE_MESSAGING
#include <mico/messaging.h>
#endif // USE_MESSAGING

#ifdef USE_CSL2
#  include <mico/service_info.h>
#  include <mico/security/security.h>
#  include <mico/security/DomainManager.h>
#  include <mico/security/odm.h>
#endif /* USE_CSL2  */
#endif // MICO_CONF_NO_INTERCEPT

#if defined(USE_CSL2) && defined(MICO_CONF_NO_INTERCEPT)
// this is needed because of POA dependency
// on ObjectDomainMapping interfaces
// when CSL2 is enabled and when MICO_CONF_NO_INTERCEPT
// is defined, then CSL2 headers are not included
namespace ObjectDomainMapping
{
    class Factory;
    class Manager;
    typedef Factory* Factory_ptr;
    typedef Manager* Manager_ptr;
}
#endif // USE_CSL2 && !MICO_CONF_NO_INTERCEPT

#include <mico/poa.h>
#include <mico/poa_base.h>
#include <mico/poa_stubs.h>
#endif // MICO_CONF_NO_POA

#include <mico/tckind.h>
#include <mico/ir_base.h>
#include <mico/typecode.h>
#include <mico/typecode_seq.h>
#include <mico/valuetype.h>

#ifndef MICO_CONF_NO_IR
#include <mico/ir.h>
#endif // MICO_CONF_NO_IR
#include <mico/service_info.h>
#include <mico/orb_mico.h>
#ifndef MICO_CONF_NO_IMR
#include <mico/imr.h>
#endif // MICO_CONF_NO_IMR
#include <mico/valuetype_impl.h>


/********************** Global ******************************************/

#ifndef MICO_CONF_NO_DYNANY
#include <mico/dynany.h>
#endif

#ifndef MICO_CONF_NO_INTERCEPT
#include <mico/pi.h>
namespace PortableInterceptor
{
    void register_orb_initializer (PortableInterceptor::ORBInitializer_ptr init);
}
#endif

#include <mico/operators.h>
#include <mico/policy2.h>

// used in skeletons
CORBA::ULong mico_string_hash (const char *, CORBA::ULong);

#include <mico/throw.h>
#include <mico/template_impl.h>

#ifndef MICO_CONF_NO_INTERCEPT
#ifdef USE_CSL2
#  include <mico/security/securitylevel1.h>
#  include <mico/security/securitylevel2.h>
#  include <mico/security/SecurityAdmin.h>
#  include <mico/security/DomainManager.h>
#  include <mico/security/odm.h>
#endif /* USE_CSL2  */
#endif // MICO_CONF_NO_INTERCEPT

#ifndef MICO_CONF_NO_INTERCEPT
#ifdef USE_CSIV2
#include <mico/security/csi.h>
#include <mico/security/csiiop.h>
#include <mico/security/gssup.h>
#include <mico/security/csiv2.h>
#endif // USE_CSIV2

#ifdef USE_SL3
// ATLAS
#include <mico/security/atlas.h>
// SL3
namespace TransportSecurity
{
    class ClientCredentials;
    class TargetCredentials;
    typedef ClientCredentials* ClientCredentials_ptr;
    typedef TargetCredentials* TargetCredentials_ptr;
}

namespace SecurityLevel3
{
    class ClientCredentials;
    class TargetCredentials;
    typedef ClientCredentials* ClientCredentials_ptr;
    typedef TargetCredentials* TargetCredentials_ptr;
}

#include <mico/security/sl3pm.h>
#include <mico/security/sl3cm.h>
#include <mico/security/sl3om.h>
#include <mico/security/sl3aqargs.h>
#include <mico/security/securitylevel3.h>
#include <mico/security/transportsecurity.h>
#include <mico/security/sl3tcpip.h>
#include <mico/security/sl3tls.h>
#include <mico/security/userpassword.h>
#include <mico/security/trust.h>
#include <mico/security/sl3authorization.h>
#include <mico/security/sl3csi.h>
#include <mico/security/sl3ipc.h>
// private SL3 API
#include <mico/security/sl3aqargs_p.h>
#include <mico/security/transportsecurity_p.h>
// MICO specific extension to SL3 API
#include <mico/security/sl3omext.h>
// MICO specific SL3 command-line API
#include <mico/security/sl3cmdext.h>
#endif // USE_SL3
#endif // MICO_CONF_NO_INTERCEPT
#endif // __corba_h__
