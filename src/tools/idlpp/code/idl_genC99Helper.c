/*
 *                         Vortex OpenSplice
 *
 *   This software and documentation are Copyright 2006 to TO_YEAR ADLINK
 *   Technology Limited, its affiliated companies and licensors. All rights
 *   reserved.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */
#include "idl_genC99Helper.h"
#include "idl_genCHelper.h"
#include "idl_tmplExp.h"
#include "idl_keyDef.h"

#include "os_heap.h"
#include "os_stdlib.h"

static c_char *idl_scopedC99SequenceTypeIdent (const idl_typeSpec typeSpec);

/** @todo Correct comment */

/* Return the scoped type specification where for the user types,
 * scopes are separated by "_" chracters.
 * IDL strings (bounded and unbounded) are mapped on: c_string, other
 *      basic types are mapped on corresponding splice types
 * IDL structures are identified by: struct <scoped-struct-name>
 * IDL unions are identified by: struct <scoped-union-name> becuase
 * the union mapping is:
 *      struct <union-name> {
 *          <tag-type> _d;
 *          union {
 *              <union-case-specifications>
 *          } _u;
 *      }
 * IDL enumerations are identified by: enum <scoped-enum-name>
 * IDL typedefs are formed by the scoped type name
 * IDL sequences are mapped on: c_sequence
 */
c_char *
idl_scopedC99TypeIdent (
    const idl_typeSpec typeSpec)
{
    c_char scopedTypeIdent[256];
    c_char *ident;

    /* QAC EXPECT 3416; No unexpected side effects here */
    if (idl_typeSpecType(typeSpec) == idl_tbasic ||
            idl_typeSpecType(typeSpec) == idl_tstruct ||
            idl_typeSpecType(typeSpec) == idl_tunion ||
            idl_typeSpecType(typeSpec) == idl_tenum) {
        /* QAC EXPECT 3416; No unexpected side effects here */
        ident = idl_c99TypeFromTypeSpec(idl_typeSpec(typeSpec));
        snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "%s", ident);
        os_free(ident);
        /* QAC EXPECT 3416; No unexpected side effects here */
    } else if (idl_typeSpecType(typeSpec) == idl_ttypedef) {
        ident = idl_scopeStack (
                    idl_typeUserScope(idl_typeUser(typeSpec)),
                    "_",
                    idl_typeSpecName(idl_typeSpec(typeSpec)));
        snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "%s", ident);
        os_free(ident);
        /* QAC EXPECT 3416; No unexpected side effects here */
    } else if (idl_typeSpecType(typeSpec) == idl_tseq) {
        ident = idl_c99SequenceIdent(idl_typeSeq(typeSpec));
        snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "%s", ident);
        os_free(ident);
    } else {
        /* Do nothing, only to prevent dangling else-ifs QAC messages */
    }

    return os_strdup(scopedTypeIdent);
}

/* Return the standalone C specific type identifier for the
 * specified type specification
 */
c_char *
idl_c99TypeFromTypeSpec (
    const idl_typeSpec typeSpec)
{
    c_char *typeName = NULL;

    /* QAC EXPECT 3416; No side effects here */
    if (idl_typeSpecType(typeSpec) == idl_tbasic) {
        /* if the specified type is a basic type */
    switch (idl_typeBasicType(idl_typeBasic(typeSpec))) {
    case idl_short:
        typeName = os_strdup("int16_t");
        break;
    case idl_ushort:
        typeName = os_strdup("uint16_t");
        break;
    case idl_long:
        typeName = os_strdup("int32_t");
        break;
    case idl_ulong:
        typeName = os_strdup("uint32_t");
        break;
    case idl_longlong:
        typeName = os_strdup("int64_t");
        break;
    case idl_ulonglong:
        typeName = os_strdup("uint64_t");
        break;
    case idl_float:
        typeName = os_strdup("float");
        break;
    case idl_double:
        typeName = os_strdup("double");
        break;
    case idl_char:
        typeName = os_strdup("char");
        break;
    case idl_string:
        typeName = os_strdup("char *");
        break;
    case idl_boolean:
        typeName = os_strdup("bool");
        break;
    case idl_octet:
        typeName = os_strdup("uint8_t");
        break;
    default:
        /* No processing required, empty statement to satisfy QAC */
        break;
    /* QAC EXPECT 2016; Default case must be empty here */
    }
        /* QAC EXPECT 3416; No side effects here */
    } else if (idl_typeSpecType(typeSpec) == idl_tseq) {
        /* sequence does not have an identification */
        typeName = os_strdup ("");
        printf ("idl_c99TypeFromTypeSpec: Unexpected sequence type handled\n");
    } else if (idl_typeSpecType(typeSpec) == idl_tarray) {
        typeName = os_strdup ("");
        printf ("idl_c99TypeFromTypeSpec: Unexpected array type handled\n");
    } else {
        /* if a user type is specified build it from its scope and its name.
         * The type should be one of idl_ttypedef, idl_tenum, idl_tstruct,
         * idl_tunion.
         */
        typeName = idl_scopeStackC (
            idl_typeUserScope(idl_typeUser(typeSpec)),
            "_",
            idl_typeSpecName(typeSpec));
    }
    return typeName;
    /* QAC EXPECT 5101; The switch statement is simple, therefor the total complexity is low */
}


/* Return the scoped actual type specification where for the user types,
 * scopes are separated by "_" chracters.
 * IDL strings (bounded and unbounded) are mapped on: c_string, other
 *      basic types are mapped on corresponding splice types
 * IDL structures are identified by: struct <scoped-struct-name>
 * IDL unions are identified by: struct <scoped-union-name> becuase
 * the union mapping is:
 *      struct <union-name> {
 *              <tag-type> _d;
 *              union {
 *                <union-case-specifications>
 *              } _u;
 *      }
 * IDL enumerations are identified by: enum <scoped-enum-name>
 * IDL typedefs are formed by the scoped type name
 * IDL sequences are mapped on: c_sequence
 */
static c_char *
idl_scopedC99SequenceElementTypeIdent (
    const idl_typeSpec typeSpec)
{
    c_char *typeName;

    /* QAC EXPECT 3416; No unexpected side effects here */
    if (idl_typeSpecType(typeSpec) == idl_tbasic) {
        switch (idl_typeBasicType(idl_typeBasic(typeSpec))) {
        case idl_short:
            typeName = os_strdup("int16_t");
            break;
        case idl_ushort:
            typeName = os_strdup("uint16_t");
            break;
        case idl_long:
            typeName = os_strdup("int32_t");
            break;
        case idl_ulong:
            typeName = os_strdup("uint32_t");
            break;
        case idl_longlong:
            typeName = os_strdup("int64_t");
            break;
        case idl_ulonglong:
            typeName = os_strdup("uint64_t");
            break;
        case idl_float:
            typeName = os_strdup("float");
            break;
        case idl_double:
            typeName = os_strdup("double");
            break;
        case idl_char:
            typeName = os_strdup("char");
            break;
        case idl_string:
            typeName = os_strdup("char *");
            break;
        case idl_boolean:
            typeName = os_strdup("bool");
            break;
        case idl_octet:
            typeName = os_strdup("uint8_t");
            break;
        default:
            typeName = os_strdup("");
        }
    } else if (idl_typeSpecType(typeSpec) == idl_tstruct ||
            idl_typeSpecType(typeSpec) == idl_tunion ||
            idl_typeSpecType(typeSpec) == idl_tenum) {
        /* QAC EXPECT 3416; No unexpected side effects here */
        typeName = idl_c99TypeFromTypeSpec(idl_typeSpec(typeSpec));
        /* QAC EXPECT 3416; No unexpected side effects here */
    } else if (idl_typeSpecType(typeSpec) == idl_ttypedef) {
        typeName = idl_c99TypeFromTypeSpec(typeSpec);
        /* QAC EXPECT 3416; No unexpected side effects here */
    } else if (idl_typeSpecType(typeSpec) == idl_tarray) {
        typeName = idl_scopedC99SequenceTypeIdent (idl_typeArrayActual(idl_typeArray(typeSpec)));
    } else if (idl_typeSpecType(typeSpec) == idl_tseq) {
        typeName = idl_c99SequenceIdent(idl_typeSeq(typeSpec));
    } else {
        typeName = NULL;
    }

    return typeName;
}

/* Return the scoped actual type specification where for the user types,
   scopes are separated by "_" characters.
   IDL strings (bounded and unbounded) are mapped on: c_string, other
       basic types are mapped on corresponding splice types
   IDL structures are identified by: struct <scoped-struct-name>
   IDL unions are identified by: struct <scoped-union-name> because
   the union mapping is:
       struct <union-name> {
           <tag-type> _d;
	   union {
		<union-case-specifications>
	   } _u;
	}
    IDL enumerations are identified by: enum <scoped-enum-name>
    IDL typedefs are formed by the scoped type name
    IDL sequences are mapped on: c_sequence
*/
static c_char *
idl_scopedC99SequenceSubElementTypeIdent (
    const idl_typeSpec typeSpec)
{
    c_char scopedTypeIdent[256];
    c_char *ident;

    /* QAC EXPECT 3416; No unexpected side effects here */
    if (idl_typeSpecType(typeSpec) == idl_tbasic) {
	switch (idl_typeBasicType(idl_typeBasic(typeSpec))) {
	case idl_short:
	    snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "short");
	    break;
	case idl_ushort:
	    snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "unsigned_short");
	    break;
	case idl_long:
	    snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "long");
	    break;
	case idl_ulong:
	    snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "unsigned_long");
	    break;
	case idl_longlong:
	    snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "long_long");
	    break;
	case idl_ulonglong:
	    snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "unsigned_long_long");
	    break;
	case idl_float:
	    snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "float");
	    break;
	case idl_double:
	    snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "double");
	    break;
	case idl_char:
	    snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "char");
	    break;
	case idl_string:
	    snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "string");
	    break;
	case idl_boolean:
	    snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "boolean");
	    break;
	case idl_octet:
	    snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "octet");
	    break;
	default:
	   os_strncpy (scopedTypeIdent, "", sizeof(scopedTypeIdent));
	}
    } else if (idl_typeSpecType(typeSpec) == idl_tstruct ||
            idl_typeSpecType(typeSpec) == idl_tunion ||
            idl_typeSpecType(typeSpec) == idl_tenum) {
        /* QAC EXPECT 3416; No unexpected side effects here */
        ident = idl_c99TypeFromTypeSpec(idl_typeSpec(typeSpec));
        snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "%s", ident);
        os_free(ident);
        /* QAC EXPECT 3416; No unexpected side effects here */
    } else if (idl_typeSpecType(typeSpec) == idl_ttypedef) {
        switch (idl_typeSpecType(idl_typeDefRefered (idl_typeDef(typeSpec)))) {
        case idl_tarray:
        case idl_tseq:
            ident = idl_c99TypeFromTypeSpec(typeSpec);
            snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "%s", ident);
            os_free(ident);
            break;
        case idl_ttypedef:
            ident = idl_scopedC99SequenceTypeIdent(idl_typeDefRefered (idl_typeDef(typeSpec)));
            snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "%s", ident);
            os_free(ident);
            break;
        default:
            ident = idl_c99TypeFromTypeSpec(idl_typeDefActual(idl_typeDef(typeSpec)));
            snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "%s", ident);
            os_free(ident);
        }
        /* QAC EXPECT 3416; No unexpected side effects here */
    } else if (idl_typeSpecType(typeSpec) == idl_tarray) {
        ident = idl_scopedC99SequenceTypeIdent (idl_typeArrayActual(idl_typeArray(typeSpec)));
        snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "%s", ident);
        os_free(ident);
    } else if (idl_typeSpecType(typeSpec) == idl_tseq) {
        ident = idl_c99SequenceIdent(idl_typeSeq(typeSpec));
        snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "%s", ident);
        os_free(ident);
    } else {
        /* Do nothing, only to prevent dangling else-ifs QAC messages */
    }

    return os_strdup(scopedTypeIdent);
}

/* Return the scoped actual type specification where for the user types,
 * scopes are separated by "_" chracters.
 * IDL strings (bounded and unbounded) are mapped on: c_string, other
 *      basic types are mapped on corresponding splice types
 * IDL structures are identified by: struct <scoped-struct-name>
 * IDL unions are identified by: struct <scoped-union-name> becuase
 * the union mapping is:
 *      struct <union-name> {
 *              <tag-type> _d;
 *              union {
 *                  <union-case-specifications>
 *              } _u;
 *      }
 * IDL enumerations are identified by: enum <scoped-enum-name>
 * IDL typedefs are formed by the scoped type name
 * IDL sequences are mapped on: c_sequence
 */

static c_char *
idl_scopedC99SequenceTypeIdent (
    const idl_typeSpec typeSpec)
{
    c_char scopedTypeIdent[256];
    c_char *ident;

    /* QAC EXPECT 3416; No unexpected side effects here */
    if (idl_typeSpecType(typeSpec) == idl_tbasic) {
	switch (idl_typeBasicType(idl_typeBasic(typeSpec))) {
	case idl_short:
	    snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "short");
	    break;
	case idl_ushort:
	    snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "unsigned_short");
	    break;
	case idl_long:
	    snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "long");
	    break;
	case idl_ulong:
	    snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "unsigned_long");
	    break;
	case idl_longlong:
	    snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "long_long");
	    break;
	case idl_ulonglong:
	    snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "unsigned_long_long");
	    break;
	case idl_float:
	    snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "float");
	    break;
	case idl_double:
	    snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "double");
	    break;
	case idl_char:
	    snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "char");
	    break;
	case idl_string:
	    snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "string");
	    break;
	case idl_boolean:
	    snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "boolean");
	    break;
	case idl_octet:
	    snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "octet");
	    break;
	default:
	   os_strncpy (scopedTypeIdent, "", sizeof(scopedTypeIdent));
	}
    } else if (idl_typeSpecType(typeSpec) == idl_tstruct ||
            idl_typeSpecType(typeSpec) == idl_tunion ||
            idl_typeSpecType(typeSpec) == idl_tenum) {
        /* QAC EXPECT 3416; No unexpected side effects here */
        ident = idl_c99TypeFromTypeSpec(idl_typeSpec(typeSpec));
        snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "%s", ident);
        os_free(ident);
        /* QAC EXPECT 3416; No unexpected side effects here */
    } else if (idl_typeSpecType(typeSpec) == idl_ttypedef) {
        ident = idl_c99TypeFromTypeSpec(typeSpec);
        snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "%s", ident);
        os_free(ident);
        /* QAC EXPECT 3416; No unexpected side effects here */
    } else if (idl_typeSpecType(typeSpec) == idl_tarray) {
        ident = idl_scopedC99SequenceTypeIdent (idl_typeArrayActual(idl_typeArray(typeSpec)));
        snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "%s", ident);
        os_free(ident);
    } else if (idl_typeSpecType(typeSpec) == idl_tseq) {
        ident = idl_c99SequenceIdent(idl_typeSeq(typeSpec));
        snprintf (scopedTypeIdent, sizeof(scopedTypeIdent), "%s", ident);
    } else {
        /* Do nothing, only to prevent dangling else-ifs QAC messages */
    }

    return os_strdup(scopedTypeIdent);
}



static char *
idl_c99SequenceSubElementIdent (
    const idl_typeSpec typeSpec)
{
    char *sequenceType;
    char *sequenceName;
    os_size_t len;

    if (idl_typeSpecType(typeSpec) == idl_tseq) {
        sequenceType = idl_c99SequenceSubElementIdent (idl_typeSeqType (idl_typeSeq(typeSpec)));
        len = strlen(sequenceType) + strlen ("sequence_") + 1;
        sequenceName = os_malloc (len);
        snprintf (sequenceName, len, "%s%s", "sequence_", sequenceType);
        os_free(sequenceType);
    } else {
        sequenceName = idl_scopedC99SequenceSubElementTypeIdent(typeSpec);
    }
    return sequenceName;
}

char *
idl_c99SequenceElementIdent (
    const idl_typeSpec typeSpec)
{
    char *sequenceType;
    char *sequenceName;
    os_size_t len;

    if (idl_typeSpecType(typeSpec) == idl_tseq) {
        sequenceType = idl_c99SequenceSubElementIdent (idl_typeSeqType (idl_typeSeq(typeSpec)));
        len = strlen(sequenceType) + strlen ("DDS_sequence_") + 1;
        sequenceName = os_malloc (len);
        snprintf (sequenceName, len, "%s%s", "DDS_sequence_", sequenceType);
        os_free(sequenceType);
    } else {
        sequenceName = idl_scopedC99SequenceElementTypeIdent(typeSpec);
    }
    return sequenceName;
}

static char *
idl_c99SequenceSubIdent (
    const idl_typeSeq typeSeq)
{
    char *sequenceType;
    char *sequenceName;
    os_size_t len;

    if (idl_typeSpecType(idl_typeSeqType (typeSeq)) == idl_tseq) {
        sequenceType = idl_c99SequenceSubIdent (idl_typeSeq(idl_typeSeqType (typeSeq)));
        len = strlen(sequenceType) + strlen ("sequence_") + 1;
        sequenceName = os_malloc (len);
        snprintf (sequenceName, len, "%s%s", "sequence_", sequenceType);
        os_free(sequenceType);
    } else {
        sequenceType = idl_scopedC99SequenceTypeIdent(idl_typeSeqType (typeSeq));
        len = strlen(sequenceType) + strlen ("sequence_") + 1;
        sequenceName = os_malloc (len);
        snprintf (sequenceName, len, "%s%s", "sequence_", sequenceType);
        os_free(sequenceType);
    }
    return sequenceName;
}


char *
idl_c99SequenceIdent (
    const idl_typeSeq typeSeq)
{
    char *sequenceType;
    char *sequenceName;
    os_size_t len;

    if (idl_typeSpecType(idl_typeSeqType (typeSeq)) == idl_tseq) {
        sequenceType = idl_c99SequenceSubIdent (idl_typeSeq(idl_typeSeqType (typeSeq)));
        len = strlen(sequenceType) + strlen ("DDS_sequence_") + 1;
        sequenceName = os_malloc (len);
        snprintf (sequenceName, len, "%s%s", "DDS_sequence_", sequenceType);
        os_free(sequenceType);
    } else {
        sequenceType = idl_scopedC99SequenceTypeIdent(idl_typeSeqType (typeSeq));
        len = strlen(sequenceType) + strlen ("DDS_sequence_") + 3;
        sequenceName = os_malloc (len);
        snprintf (sequenceName, len, "%s%s", "DDS_sequence_", sequenceType);
        os_free(sequenceType);
    }
    return sequenceName;
}

char *
idl_c99SequenceIdentScoped (
    const idl_scope scope,
    const idl_typeSeq typeSeq)
{
    char *sequenceType;
    char *sequenceName;
    idl_scopeElement scopeElem;
    c_long i;
    int n;
    int pos;
    os_size_t len = 0;

    for (i = 0; i < idl_scopeStackSize(scope); i++) {
        scopeElem = idl_scopeIndexed(scope, i);
        len += strlen(idl_scopeElementName(scopeElem)) + 1;
    }

    if (idl_typeSpecType(idl_typeSeqType (typeSeq)) == idl_tseq) {
        sequenceType = idl_c99SequenceSubIdent (idl_typeSeq(idl_typeSeqType (typeSeq)));
    } else {
        sequenceType = idl_scopedC99SequenceTypeIdent(idl_typeSeqType (typeSeq));
    }

    len += strlen(sequenceType) + strlen ("_sequence_") + 1;
    sequenceName = os_malloc (len);
    pos = 0;

    for (i = 0; i < idl_scopeStackSize(scope); i++) {
        scopeElem = idl_scopeIndexed(scope, i);
        n = snprintf (&sequenceName[pos], len, "%s_", idl_scopeElementName(scopeElem));
        pos += n;
        len -= (os_size_t)n;
    }

    snprintf (&sequenceName[pos], len, "%s%s", "sequence_", sequenceType);

    os_free(sequenceType);

    return sequenceName;
}


c_bool
idl_c99SequenceSupportFunctionsExist (
    const idl_scope scope,
    const idl_typeSeq typeSeq,
    const char *elementName)
{
    c_bool exist = FALSE;
    idl_scope typeScope;

    typeScope = idl_typeUserScope(idl_typeUser(idl_typeSpecActual(idl_typeSeqType(typeSeq))));
    if (typeScope != NULL) {
        char *typeBase = idl_scopeBasename(typeScope);
        char *workBase = idl_scopeBasename(scope);

        if (strcmp(typeBase, workBase) != 0) {

            /* If a type has a related key, that means that readers and
             * writers have been created, which also means that type
             * sequence support functions have already been created in
             * the other context.
             */
            if (idl_keyDefIncludesType(idl_keyDefDefGet(), elementName)) {
                exist = TRUE;
            }

            /* We should search all scopes of all contexts to be sure that
             * no sequence support functions for this type are defined
             * anywhere within the hierarchy by means of an actual
             * 'sequence<Type>' definition in an idl file.
             * We shoudn't only search the context of the type itself for
             * sequences of this type because there could be a file include
             * between the current working context and the type definition
             * context, in which a sequence can be defined.
             *
             * We don't have access to all contexts at this point.
             * Also, only the scope information isn't enough to detect
             * sequences. So, basically, we don't have enough information
             * here to do a proper search.
             *
             * This multiple sequences problem, however, can be solved by
             * adding a sequence typedef in the idl hierarchy and use that
             * typedef everywhere else.
             */
        }
        os_free(typeBase);
        os_free(workBase);
    }

    return exist;
}
