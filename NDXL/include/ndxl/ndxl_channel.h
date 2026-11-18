/**
 * @file ndxl_channel.h
 *
 * @brief Public API for NDXL noise channel creation and querying.
 *
 * Declares the NDXLChannel object, representing an independent Wiener process. Provides functions for channel construction, destruction, and querying.
 *
 * @copyright Copyright (c) 2026 Ethan Codron
 * @license GNU GPL-3.0-only
*/

#ifndef _NDXL_CHANNEL_
#define _NDXL_CHANNEL_

#include "../ndxlcore.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup channel_creation Noise Channel Constructor and Destructor
 * @brief Functions for creating and destroying noise channels.
 *
 * @{
*/

/**
 * @brief Creates a noise channel in a network and returns a pointer to it.
 *
 * @param _Network
 * Network that will own this channel. This also dictates which vertices and edges can be affected by this channel.
 * 
 * @param _Params
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
 * @see ndxlEdgeVertex
*/
_NDXLIMP_ NDXLChannel* _CDECL ndxlCreateChannel(
	_In_ _Notnull_ NDXLNetwork* _Network,
	_In_ _Maybenull_ void* _Params
) _NOEXCEPT;

/**
 * @brief Destroys a noise channel created using ndxlCreateChannel(), and zeroes the pointer.
 *
 * @param _Channel
 * Pointer to NDXLChannel pointer.
 *
 * @return
 * If _Channel or *_Channel are NULL, returns NDXL_ERR, otherwise zeroes *_Channel, and returns NDXL_OK.
 *
 * @note
 * The channel pointer is zeroed after the associated memory is freed, so that subsequent operations using the pointer variable passed to this function can detect that the channel has been destroyed.
 *
 * @warning
 * Destroying a noise channel also destroys its associated Wiener path. Any vertices and edges referencing the channel automatically remove their references to it and continue to exist without using that channel. The memory associated with the relevant objects' user-defined `void* _Params` pointers is not handled by the destructors, and must be managed explicitly by the user.
 *
 * However, destroying a noise channel while it is in use can disrupt Wiener path preservation in rejection sampling with memory (RSwM) algorithms, potentially introducing statistical bias into subsequent simulations.
 *
 * It is therefore strongly recommended to verify that both ndxlGetChannelEdgeCount() and ndxlGetChannelVertexCount() return zero before destroying a noise channel.
 * 
 * Only the pointer variable passed to this function is zeroed. Any other pointers referencing the destroyed channel become invalid and must not be used.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Channel is NULL.
 * - NDXL_VALUE_ERR: *_Channel is NULL.
 *
 * @see ndxlCreateChannel
*/
_NDXLIMP_ const int _CDECL ndxlDestroyChannel(
	_In_ _Notnull_ NDXLChannel** _Channel
) _NOEXCEPT;

/** @}*/



/**
 * @defgroup channel_getters Noise Channel Getters
 * @brief Getter functions for querying noise channel properties.
 *
 * @{
*/

/**
 * @brief Gets the number of edges affected by a noise channel.
 *
 * @param _Channel
 * Target channel to retrieve the edge count of.
 *
 * @return
 * If _Channel is NULL, returns NDXL_ERR, otherwise returns the number of edges affected by _Channel.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Channel is NULL.
*/
_NDXLIMP_ const size_t _CDECL ndxlGetChannelEdgeCount(
	_In_ _Notnull_ NDXLChannel* _Channel
) _NOEXCEPT;

/**
 * @brief Gets a list of all the edges affected by a noise channel.
 *
 * @param _Channel
 * Target noise channel to retrieve the affected edges of.
 *
 * @param _Edges
 * Pointer to a user-allocated array of NDXLEdge pointers. The array must contain at least ndxlGetChannelEdgeCount(_Channel) elements. The function populates the array in-place.
 *
 * @return
 * If either _Channel or _Edges are NULL, returns NDXL_ERR, otherwise populates _Edges with the edge pointers affected by _Channel, and returns NDXL_OK.
 *
 * @note
 * The caller retains ownership of the array, but not of the NDXLEdge objects referenced by its elements. These pointers refer to the original objects in the network, so modifications through them affect those objects directly. The returned list can be used to modify or destroy its referenced objects without invalidating the array itself.
 *
 * @warning
 * The function does not verify the capacity of the supplied array. Providing an inadequately sized array can result in out-of-bounds writes and undefined behaviour.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Channel is NULL.
 * - NDXL_ARG_ERR: _Edges is NULL.
 *
 * @see ndxlGetChannelEdgeCount
*/
_NDXLIMP_ const int _CDECL ndxlGetChannelEdges(
	_In_ _Notnull_ NDXLChannel* _Channel,
	_Out_ _Notnull_ NDXLEdge** _Edges
) _NOEXCEPT;

/**
 * @brief Gets the owning network of a noise channel.
 *
 * @param _Channel
 * Target noise channel to retrieve the network of.
 *
 * @return
 * If _Channel is NULL, returns NULL, otherwise returns the NDXLNetwork pointer on which _Channel was created.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Channel is NULL.
*/
_NDXLIMP_ NDXLNetwork* _CDECL ndxlGetChannelNetwork(
	_In_ _Notnull_ NDXLChannel* _Channel
) _NOEXCEPT;

/**
 * @brief Gets the user-defined `void* _Params` of a noise channel.
 *
 * @param _Channel
 * Target channel to retrieve the parameters of.
 *
 * @return
 * If _Channel is NULL, returns NULL, otherwise returns the user-defined `void* _Params` of _Channel.
 *
 * @note
 * A return value of NULL does not necessarily indicate an error, as `void* _Params` could be NULL. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Channel is NULL.
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
 * @param _Channel
 * Target channel to retrieve the vertex count of.
 *
 * @return
 * If _Channel is NULL, returns NDXL_ERR, otherwise returns the number of vertices affected by _Channel.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Channel is NULL.
*/
_NDXLIMP_ const size_t _CDECL ndxlGetChannelVertexCount(
	_In_ _Notnull_ NDXLChannel* _Channel
) _NOEXCEPT;

/**
 * @brief Gets a list of all the vertices affected by a noise channel.
 *
 * @param _Channel
 * Target noise channel to retrieve the affected vertices of.
 *
 * @param _Vertices
 * Pointer to a user-allocated array of NDXLVertex pointers. The array must contain at least ndxlGetChannelVertexCount(_Channel) elements. The function populates the array in-place.
 *
 * @return
 * If either _Channel or _Vertices are NULL, returns NDXL_ERR, otherwise populates _Vertices with the vertex pointers affected by _Channel, and returns NDXL_OK.
 *
 * @note
 * The caller retains ownership of the array, but not of the NDXLVertex objects referenced by its elements. These pointers refer to the original objects in the network, so modifications through them affect those objects directly. The returned list can be used to modify or destroy its referenced objects without invalidating the array itself.
 *
 * @warning
 * The function does not verify the capacity of the supplied array. Providing an inadequately sized array can result in out-of-bounds writes and undefined behaviour.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Channel is NULL.
 * - NDXL_ARG_ERR: _Vertices is NULL.
 *
 * @see ndxlGetChannelVertexCount
*/
_NDXLIMP_ const int _CDECL ndxlGetChannelVertices(
	_In_ _Notnull_ NDXLChannel* _Channel,
	_Out_ _Notnull_ NDXLVertex** _Vertices
) _NOEXCEPT;

/** @}*/



/**
 * @defgroup channel_setters Channel Setters
 * @brief Setter functions for updating channel properties.
 *
 * @{
*/

/**
 * @brief Sets the user-defined `void* _Params` of a noise channel.
 *
 * @param _Channel
 * Target channel to update the parameters of.
 *
 * @param _Params
 * Pointer to extra memory used by the channel. May be NULL.
 *
 * @return
 * If _Channel is NULL, returns NDXL_ERR, otherwise updates the parameters of _Channel, and returns NDXL_OK.
 *
 * @warning
 * NDXL stores the supplied `void* _Params` pointer without copying the associated data or managing its lifetime. The pointed-to memory must remain valid for as long as the channel uses it.
 *
 * This function replaces the channel's existing parameter pointer without freeing or otherwise managing the memory it previously referenced. The user remains responsible for managing both the previous and new allocations.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Channel is NULL.
 *
 * @see ndxlGetChannelParams
*/
_NDXLIMP_ const int _CDECL ndxlSetChannelParams(
	_In_ _Notnull_ NDXLChannel* _Channel,
	_In_ _Maybenull_ void* _Params
) _NOEXCEPT;

/** @}*/

#ifdef __cplusplus
}
#endif

#endif // _NDXL_CHANNEL_