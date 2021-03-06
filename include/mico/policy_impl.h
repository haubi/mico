// -*- c++ -*-
/*
 *  MICO --- an Open Source CORBA implementation
 *  Copyright (C) 1998 Kay Roemer & Arno Puder
 *  Copyright (c) 1999-2008 by The Mico Team
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

#ifndef __POLICY_IMPL_H__
#define __POLICY_IMPL_H__

namespace MICO {

class Policy_impl : virtual public CORBA::Policy {
    CORBA::PolicyType pt;
protected:
    Policy_impl (CORBA::PolicyType);
#ifdef USE_CSL2
    Policy_impl () {};
#endif /* USE_CSL2  */

public:
    virtual ~Policy_impl ();
    CORBA::PolicyType policy_type ();
    void destroy ();
};

class PolicyManager_impl
    : virtual public CORBA::PolicyManager,
      virtual public CORBA::LocalObject
{
public:
    PolicyManager_impl();

    virtual
    ~PolicyManager_impl()
    {}

    virtual CORBA::PolicyList*
    get_policy_overrides(const CORBA::PolicyTypeSeq& ts);

    virtual void
    set_policy_overrides(const CORBA::PolicyList& policies, CORBA::SetOverrideType set_add);
private:
    CORBA::PolicyList policies_;
    MICOMT::Mutex policies_mutex_;
};

class PolicyCurrent_impl
    : virtual public CORBA::PolicyCurrent,
      virtual public CORBA::LocalObject
{
public:
    PolicyCurrent_impl();

    virtual
    ~PolicyCurrent_impl();

    virtual CORBA::PolicyList*
    get_policy_overrides(const CORBA::PolicyTypeSeq& ts);

    virtual void
    set_policy_overrides(const CORBA::PolicyList& policies, CORBA::SetOverrideType set_add);
private:
    PolicyManager_impl*
    get_current_manager(CORBA::Boolean create);

private:
    CORBA::ORB_var orb_;
#ifndef HAVE_THREADS
    PolicyManager_impl* manager_;
#else // HAVE_THREADS
    MICOMT::Thread::ThreadKey policy_current_key_;
#endif // HAVE_THREADS
};

class DomainManager_impl : virtual public CORBA::DomainManager {
    // inherits _policies from CORBA::Object
    // inherits _managers from CORBA::Object
public:
    DomainManager_impl ();
    virtual ~DomainManager_impl ();

    CORBA::Policy_ptr get_domain_policy (CORBA::PolicyType policy_type);
    void set_domain_policy (CORBA::Policy_ptr policy);
    CORBA::DomainManager_ptr copy ();
};

class ConstructionPolicy_impl : virtual public Policy_impl,
				virtual public CORBA::ConstructionPolicy {
    typedef std::map<std::string, CORBA::Boolean, std::less<std::string> >
    IdConstrMap;
    IdConstrMap _constr;
public:
    ConstructionPolicy_impl ();
    virtual ~ConstructionPolicy_impl ();

    void make_domain_manager (CORBA::InterfaceDef_ptr object_type,
			      CORBA::Boolean constr_policy);

    CORBA::Boolean constr_policy (CORBA::Object_ptr object);

    CORBA::Policy_ptr copy ();

    virtual void *_narrow_helper (const char *rid)
    {
        return this->CORBA::ConstructionPolicy::_narrow_helper(rid);
    }
};

class TransportPrefPolicy_impl :
    virtual public Policy_impl,
    virtual public MICOPolicy::TransportPrefPolicy
{
    ProfileTagSeq _prefs;
public:
    TransportPrefPolicy_impl (const ProfileTagSeq &prefs);
    TransportPrefPolicy_impl ();
    virtual ~TransportPrefPolicy_impl ();

    ProfileTagSeq *preferences();
    void preferences (const ProfileTagSeq &prefs);
    ProfileTagSeq *preferences_nocopy();

    CORBA::Policy_ptr copy ();

    virtual void *_narrow_helper (const char *rid)
    {
        return this->MICOPolicy::TransportPrefPolicy::_narrow_helper(rid);
    }
};

class BidirectionalPolicy_impl :
    virtual public Policy_impl,
    virtual public BiDirPolicy::BidirectionalPolicy
{
    BiDirPolicy::BidirectionalPolicyValue _value;
public:
    BidirectionalPolicy_impl (BiDirPolicy::BidirectionalPolicyValue);
    BidirectionalPolicy_impl ();
    virtual ~BidirectionalPolicy_impl ();

    BiDirPolicy::BidirectionalPolicyValue value();

    CORBA::Policy_ptr copy ();

    virtual void *_narrow_helper (const char *rid)
    {
        return this->BiDirPolicy::BidirectionalPolicy::_narrow_helper(rid);
    }
};

#ifdef USE_MESSAGING
class RelativeConnectionBindingTimeoutPolicy_impl
    : public virtual Policy_impl,
      public virtual MICOPolicy::RelativeConnectionBindingTimeoutPolicy,
      public virtual ::CORBA::LocalObject
{
public:
    RelativeConnectionBindingTimeoutPolicy_impl(TimeBase::TimeT value);

    virtual
    ~RelativeConnectionBindingTimeoutPolicy_impl();

    virtual ::CORBA::Policy_ptr
    copy();

    virtual ::TimeBase::TimeT
    relative_expiry();
private:
    ::TimeBase::TimeT relative_expiry_;
};
#endif // USE_MESSAGING

}

#endif
