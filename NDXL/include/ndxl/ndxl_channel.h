/**
 * @file ndxl_channel.h
 *
 * @brief Public API for NDXL noise channel creation and querying.
 *
 * Declares the NDXLChannel object, representing an independent Wiener process. Provides functions for channel construction, destruction, and querying.
 *
 * @copyright Copyright (c) 2026, Ethan Codron.
 * 
 * Licensed under the GNU General Public License v3.0 only.
 * See the LICENSE file in the repository root.
*/

#ifndef _NDXL_CHANNEL_
#define _NDXL_CHANNEL_

#include "../ndxlcore.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup channel_creation Constructor and Destructor
 * @ingroup ndxl_channel
 * @brief Functions for creating and destroying noise channels.
 *
 * @{
*/

/**
 * @brief Creates a noise channel in a network and returns a pointer to it.
 *
 * @param[in] _Network
 * Network that will own this channel. This also dictates which vertices and edges can be affected by this channel.
 * 
 * @param[in] _Params
 * Pointer to extra memory used by the channel. May be NULL.
 *
 * @return
 * If _Network is NULL, returns NULL, otherwise returns a new NDXLChannel pointer.
 *
 * @note
 * Each channel maintains its own Wiener path for use in rejection sampling with memory (RSwM) algorithms. Destroying one while it is in use may disrupt path preservation and intdoduce statistical bias. See ndxlDestroyChannel() for further details.
 *
 * @warning
 * NDXL stores the supplied `void* _Params` pointer without copying the associated data or managing its lifetime. The pointed-to memory must remain valid for as long as the channel uses it. The user remains responsible for managing any allocations.
 *
 * @see ndxlDestroyChannel
 * @see ndxlGetChannelEdgeCount
 * @see ndxlGetChannelVertexCount
 * @see ndxlCreateVertex
 * @see ndxlCreateEdge
*/
_NDXLIMP_ NDXLChannel* _CDECL ndxlCreateChannel(
	_In_ _Notnull_ NDXLNetwork* _Network,
	_In_ _Maybenull_ void* _Params
) _NOEXCEPT;

/**
 * @brief Destroys a noise channel created using ndxlCreateChannel(), and zeroes the pointer.
 *
 * @param[in,out] _Channel
 * Pointer to NDXLChannel pointer.
 *
 * @return
 * If _Channel is NULL, returns NULL, otherwise destroys the channel and returns NDXL_OK.
 *
 * @warning
 * Destroying a noise channel also destroys its associated Wiener path. Any vertices and edges referencing the channel automatically remove their references to it and continue to exist without using that channel. The memory associated with the relevant objects' user-defined `void* _Params` pointers is not handled by the destructors, and must be managed explicitly by the user.
 *
 * @warning
 * However, destroying a noise channel while it is in use can disrupt Wiener path preservation in rejection sampling with memory (RSwM) algorithms, potentially introducing statistical bias into subsequent simulations.
 *
 * @warning
 * It is therefore strongly recommended to verify that both ndxlGetChannelEdgeCount() and ndxlGetChannelVertexCount() return zero before destroying a noise channel.
 * 
 * @warning
 * Only the pointer variable passed to this function is zeroed. Any other pointers referencing the destroyed channel become invalid and must not be used.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Channel is NULL.
 *
 * @see ndxlCreateChannel
*/
_NDXLIMP_ const int _CDECL ndxlDestroyChannel(
	_In_ _Notnull_ NDXLChannel* _Channel
) _NOEXCEPT;

/** @}*/



/**
 * @defgroup channel_getters Getters
 * @ingroup ndxl_channel
 * @brief Getter functions for querying noise channel properties.
 *
 * @{
*/

/**
 * @brief Gets the number of edges affected by a noise channel.
 *
 * @param[in] _Channel
 * Target channel to retrieve the edge dimension of.
 *
 * @return
 * If _Channel is NULL, returns @ref NDXL_ERR, otherwise returns the number of edges affected by _Channel.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Channel is NULL.
*/
_NDXLIMP_ const size_t _CDECL ndxlGetChannelEdgeDimension(
	_In_ _Notnull_ NDXLChannel* _Channel
) _NOEXCEPT;

/**
 * @brief Gets the owning network of a noise channel.
 *
 * @param[in] _Channel
 * Target noise channel to retrieve the network of.
 *
 * @return
 * If _Channel is NULL, returns NULL, otherwise returns the NDXLNetwork pointer on which _Channel was created.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Channel is NULL.
*/
_NDXLIMP_ NDXLNetwork* _CDECL ndxlGetChannelNetwork(
	_In_ _Notnull_ NDXLChannel* _Channel
) _NOEXCEPT;

/**
 * @brief Gets the user-defined `void* _Params` of a noise channel.
 *
 * @param[in] _Channel
 * Target channel to retrieve the parameters of.
 *
 * @return
 * If _Channel is NULL, returns NULL, otherwise returns the user-defined `void* _Params` of _Channel.
 *
 * @note
 * A return value of NULL does not necessarily indicate an error, as `void* _Params` could be NULL. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Channel is NULL.
 *
 * @see ndxlGetLastErrorCode
 * @see ndxlSetChannelParams
*/
_NDXLIMP_ void* _CDECL ndxlGetChannelParams(
	_In_ _Notnull_ NDXLChannel* _Channel
) _NOEXCEPT;

/**
 * @brief Gets the number of vertices affected by a noise channel.
 *
 * @param[in] _Channel
 * Target channel to retrieve the vertex dimension of.
 *
 * @return
 * If _Channel is NULL, returns @ref NDXL_ERR, otherwise returns the number of vertices affected by _Channel.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Channel is NULL.
*/
_NDXLIMP_ const size_t _CDECL ndxlGetChannelVertexDimension(
	_In_ _Notnull_ NDXLChannel* _Channel
) _NOEXCEPT;

/** @}*/



/**
 * @defgroup channel_setters Setters
 * @ingroup ndxl_channel
 * @brief Setter functions for updating channel properties.
 *
 * @{
*/

/**
 * @brief Sets the user-defined `void* _Params` of a noise channel.
 *
 * @param[in] _Channel
 * Target channel to update the parameters of.
 *
 * @param[in] _Params
 * Pointer to extra memory used by the channel. May be NULL.
 *
 * @return
 * If _Channel is NULL, returns @ref NDXL_ERR, otherwise updates the parameters of _Channel, and returns NDXL_OK.
 *
 * @warning
 * NDXL stores the supplied `void* _Params` pointer without copying the associated data or managing its lifetime. The pointed-to memory must remain valid for as long as the channel uses it.
 *
 * This function replaces the channel's existing parameter pointer without freeing or otherwise managing the memory it previously referenced. The user remains responsible for managing both the previous and new allocations.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Channel is NULL.
 *
 * @see ndxlGetChannelParams
*/
_NDXLIMP_ const int _CDECL ndxlSetChannelParams(
	_In_ _Notnull_ NDXLChannel* _Channel,
	_In_ _Maybenull_ void* _Params
) _NOEXCEPT;

/** @}*/



/**
 * @defgroup channel_functions Functions
 * @ingroup ndxl_channel
 * @brief Functions for operating on noise channels.
 *
 * @{
*/

/**
 * @brief Provides directed access to an edge in a channel.
 *
 * @param[in] _Channel
 * Target channel to retrieve the edge in.
 *
 * @param[in] _Idx
 * Relative index (position) of edge in the array. Must be greater than or equal to zero, and less than ndxlGetChannelEdgeDimension(_Channel).
 *
 * @return
 * If _Channel is NULL or contains no edges, or _Idx is out of bounds of the underlying array, returns NULL, otherwise returns NDXLEdge pointer currently at position _Idx in the channel's underlying edge array.
 *
 * @note
 * The pointer refers to the original object in the channel, so modifications through it affect the object directly. The returned object can be used to modify or destroy its referenced object without invalidating the object itself.
 *
 * @note
 * A return value of NULL does not necessarily indicate an error, as the channel could have no edges. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @warning
 * Relative indices are shuffled when agents are destroyed to save memory. Repeated calls with the same index will not necessarily return the same object.
 *
 * @par Errors raised
 * - @ref NDXL_ARG_ERR : _Channel is NULL.
 * - @ref NDXL_VALUE_ERR : _Idx is out of bounds.
 *
 * @see ndxlGetChannelEdgeDimension
*/
_NDXLIMP_ NDXLEdge* _CDECL ndxlChannelEdge(
	_In_ _Notnull_ NDXLChannel* _Channel,
	_In_ const size_t _Idx
) _NOEXCEPT;

/**
 * @brief Provides directed access to an vertex in a channel.
 *
 * @param[in] _Channel
 * Target channel to retrieve the vertex in.
 *
 * @param[in] _Idx
 * Relative index (position) of vertex in the array. Must be greater than or equal to zero, and less than ndxlGetChannelVertexDimension(_Channel).
 *
 * @return
 * If _Channel is NULL or contains no vertices, or _Idx is out of bounds of the underlying array, returns NULL, otherwise returns NDXLVertex pointer currently at position _Idx in the channel's underlying vertex array.
 *
 * @note
 * The pointer refers to the original object in the channel, so modifications through it affect the object directly. The returned object can be used to modify or destroy its referenced object without invalidating the object itself.
 *
 * @note
 * A return value of NULL does not necessarily indicate an error, as the channel could have no vertices. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @warning
 * Relative indices are shuffled when agents are destroyed to save memory. Repeated calls with the same index will not necessarily return the same object.
 *
 * @par Errors raised
 * - @ref NDXL_ARG_ERR : _Channel is NULL.
 * - @ref NDXL_VALUE_ERR : _Idx is out of bounds.
 *
 * @see ndxlGetChannelVertexDimension
*/
_NDXLIMP_ NDXLVertex* _CDECL ndxlChannelVertex(
	_In_ _Notnull_ NDXLChannel* _Channel,
	_In_ const size_t _Idx
) _NOEXCEPT;

/** @}*/

#ifdef __cplusplus
}
#endif

#endif // _NDXL_CHANNEL_