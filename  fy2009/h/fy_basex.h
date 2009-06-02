/* ====================================================================
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2009 The FengYi2009 Project, All rights reserved.
 *
 * Author: DreamFreelancer, zhangxb66@2008.sina.com
 *
 * [History]
 * initialize: 2009-6-1
 * ====================================================================
 */
#ifndef __FENGYI2009_BASEX_DREAMFREELANCER_20080523_H__
#define __FENGYI2009_BASEX_DREAMFREELANCER_20080523_H__ 

#include "fy_stream.h"

DECL_FY_NAME_SPACE_BEGIN

/*[tip]
 *[desc] dynamic type is ofen convenient for all-purpose applications. it's good for extensibility
 *
 *[history] 
 * Initialize: 2006-11-3,usa
 *[memo]
 * float type is seldom used in communication software, and if it's really used, you can covert it to a portable format
 * e.g. two integer or a string
 */
//variant data type.bit pattern:7bit:1:array,0:basic type, 6-0bits:basic type
//eg.VT_ARRAY|VT_I8 means real type is int8 array
const int8 VT_ARRAY =0x80;
const int8 VT_TYPE  =0x7f;

//basic type
const int8 VT_NULL =0;
const int8 VT_I8   =0x01;
const int8 VT_I16  =0x02;
const int8 VT_I32  =0x03;
const int8 VT_LOOKUP  =0x04;//which realizes lookup_it interface and generally, ref_cnt_it is often needed

class variant_t
{
public:
        variant_t();
        variant_t(int8 i8);
        variant_t(int16 i16);
        variant_t(int32 i32);
        variant_t(lookup_it *obj, bool attach_opt=true);
        variant_t(const variant_t& v);//shallow copy object value,if any
        ~variant_t();

        const variant_t& operator =(const variant_t& v);//shallow copy object value,if any
        void clone_from(const variant_t& src_v);//deep copy

        inline int8 get_type() const throw() { return _vt & VT_TYPE; }
        inline bool is_array() const throw() { return _vt & VT_ARRAY; }
        inline uint16 get_array_size() const throw()
        {
                if(_vt & VT_ARRAY)
                        return _size;
                else
                        return 0;
        }

        void set_null();
        bool is_null() const throw();

        void set_i8(int8 i8);
        int8 get_i8() const;

        void set_i8s(int8 *pi8, uint16 size, bool attach_opt=false);
        //pvsize return size of array
        int8 *get_i8s(uint16 *psize) const;

        void set_i16(int16 i16);
        int16 get_i16() const;

        void set_i16s(int16 *pi16, uint16 size, bool attach_opt=false);
        int16 *get_i16s(uint16 *psize) const;

        void set_i32(int32 i32);
        int32 get_i32() const;

        void set_i32s(int32 *pi32, uint16 size, bool attach_opt=false);
        int32 *get_i32s(uint16 *psize) const;

        void set_obj(lookup_it *obj, bool attach_opt=false);
        lookup_it *get_obj() const;

        void set_objs(lookup_it **objs, uint16 size, bool attach_opt=false);
        lookup_it **get_objs(uint16 *psize) const;

        //persist--to minimize the size of this class, don't implement persist_it directly
        uint32 calc_persist_size() const;
        void save_to(stream_adaptor_t& stm_adp);
        void load_from(stream_adaptor_t& stm_adp);
private:
        void _reset();

        //not responsible for resetting current object
        void _copy(const variant_t& v, bool shallow_copy_obj=true);
private:
        int8 _vt;
        uint16 _size;//size of array
        union
        {
                int8 i8;
                int8 *pi8;

                int16 i16;
                int16 *pi16;

                int32 i32;
                int32 *pi32;

                lookup_it *obj;
                lookup_it **objs;
        } _val;
};


/*[tip]dynamic property support
 *[desc] in most cases, dynamic property is an important feature, this interface specify this requirement generally
 *[history] 
 * Initialize: 2008-6-27
 * Revise: 2009-6-1, add fingerprint to avoid prop_id definition conflict in concurrently development
 *    integer proper id is efficient but isn't concurrent-development-safe, that is, it is easy for different developer
 *    define "different" proper id with same value, which will cause strange systemic behavior and it 's difficult to 
 *    find this bug, use more unique type proper id can resolve this issue, e.g. uuiq or string, but they are not temporal
 *    or spacial efficient, fingerprint is a beffer compromise, developer is responsible for defining an specified fingerprint
 *    for a dynamic property and responsible for checking if it's correct to find potential dynamic property definition
 *    conflict. 
 */
class dyna_prop_it : public lookup_it
{
public:
        virtual uint16 set_dprop(uint16 prop_id, uint32 fingerprint, const variant_t& prop)=0;
	
	//if fingerprint isn't zero, dynamic property specified by prop_id must have same fingerprint, otherwise, throw exception,
	//, if p_fingerprint isn't null, it will return practical fingerprint set by set_dprop
        virtual variant_t get_dprop(uint16 prop_id, uint32 fingerprint=0, uint32 *p_fingerprint=0)=0;
        virtual uint16 get_dprop_count()=0;
};

DECL_FY_NAME_SPACE_END

#endif //__FENGYI2009_BASEX_DREAMFREELANCER_20080523_H__