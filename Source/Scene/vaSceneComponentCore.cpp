///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016-2021, Intel Corporation 
// 
// SPDX-License-Identifier: MIT
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author(s):  Filip Strugar (filip.strugar@intel.com)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "vaSceneComponentCore.h"
#include "vaSceneComponents.h"
// #include "vaSceneComponentsIO.h"
// #include "vaSceneSystems.h"
// 
// #include "Rendering/vaRenderMesh.h"
// #include "Rendering/vaRenderMaterial.h"

#include "Core/System/vaFileTools.h"

#include "Core/vaSerializer.h"


using namespace Vanilla;

using namespace Vanilla::Scene;

int Components::TypeIndex( const string & name )
{
    return vaSceneComponentRegistry::GetInstance().FindComponentTypeIndex( name );
}

int Components::TypeCount( )
{
    return (int)vaSceneComponentRegistry::GetInstance().m_components.size();
}

int Components::TypeUseCount( int typeIndex, entt::registry & registry )
{
    assert( typeIndex >= 0 && typeIndex < TypeCount( ) );
    return vaSceneComponentRegistry::GetInstance( ).m_components[typeIndex].TotalCountCallback(registry);
}

const string & Components::TypeName( int typeIndex )
{
    assert( typeIndex >= 0 && typeIndex < TypeCount( ) );
    return vaSceneComponentRegistry::GetInstance( ).m_components[typeIndex].NameID;
}

string Components::DetailedTypeInfo( int typeIndex )
{
    assert( typeIndex >= 0 && typeIndex < TypeCount( ) );
    auto & typeInfo = vaSceneComponentRegistry::GetInstance( ).m_components[typeIndex];
    
    string outInfo;
    outInfo += "Component name:     " + typeInfo.NameID;                                                                        
    outInfo += "\n";
    outInfo += "C++ type name:      " + typeInfo.TypeName + ", type index: " + std::to_string( typeInfo.TypeIndex );
    outInfo += "\n";
    outInfo += "Visible in UI:      " + std::to_string( typeInfo.UIVisible );
    outInfo += "\n";
    outInfo += "Modifiable in UI:   " + std::to_string( typeInfo.UIAddRemoveResetDisabled );
    outInfo += "\n";
    outInfo += "Has serializer:     " + std::to_string( typeInfo.SerializerCallback.operator bool() );
    outInfo += "\n";
    outInfo += "Has UI handler:     " + std::to_string( typeInfo.UITickCallback.operator bool() );
    return outInfo;
}

bool Components::Has( int typeIndex, entt::registry & registry, entt::entity entity )
{
    assert( typeIndex >= 0 && typeIndex < TypeCount() );
    return vaSceneComponentRegistry::GetInstance().m_components[typeIndex].HasCallback( registry, entity );
}

void Components::EmplaceOrReplace( int typeIndex, entt::registry & registry, entt::entity entity )
{
    assert( typeIndex >= 0 && typeIndex < TypeCount( ) );
    vaSceneComponentRegistry::GetInstance( ).m_components[typeIndex].EmplaceOrReplaceCallback( registry, entity );
}

void Components::Remove( int typeIndex, entt::registry & registry, entt::entity entity )
{
    assert( typeIndex >= 0 && typeIndex < TypeCount( ) );
    vaSceneComponentRegistry::GetInstance( ).m_components[typeIndex].RemoveCallback( registry, entity );
}

bool Components::HasSerialize( int typeIndex )
{
    assert( typeIndex >= 0 && typeIndex < TypeCount( ) );
    return vaSceneComponentRegistry::GetInstance( ).m_components[typeIndex].SerializerCallback != nullptr;
}

bool Components::HasUITick( int typeIndex )
{
    assert( typeIndex >= 0 && typeIndex < TypeCount( ) );
    return vaSceneComponentRegistry::GetInstance( ).m_components[typeIndex].UITickCallback != nullptr;
}

bool Components::HasUITypeInfo( int typeIndex )
{
    assert( typeIndex >= 0 && typeIndex < TypeCount( ) );
    return vaSceneComponentRegistry::GetInstance( ).m_components[typeIndex].UITypeInfoCallback != nullptr;
}

bool Components::UIVisible( int typeIndex )
{
    assert( typeIndex >= 0 && typeIndex < TypeCount( ) );
    return vaSceneComponentRegistry::GetInstance( ).m_components[typeIndex].UIVisible;
}

bool Components::UIAddRemoveResetDisabled( int typeIndex )
{
    assert( typeIndex >= 0 && typeIndex < TypeCount( ) );
    return vaSceneComponentRegistry::GetInstance( ).m_components[typeIndex].UIAddRemoveResetDisabled;
}

bool Components::Serialize( int typeIndex, entt::registry & registry, entt::entity entity, SerializeArgs & args, class vaSerializer & serializer )
{
    assert( HasSerialize( typeIndex ) );
    return vaSceneComponentRegistry::GetInstance( ).m_components[typeIndex].SerializerCallback( registry, entity, args, serializer );
}

void Components::UITick( int typeIndex, entt::registry & registry, entt::entity entity, Scene::UIArgs & uiArgs )
{
    assert( HasUITick( typeIndex ) );
    return vaSceneComponentRegistry::GetInstance( ).m_components[typeIndex].UITickCallback( registry, entity, uiArgs );
}

const char * Components::UITypeInfo( int typeIndex )
{
    assert( HasUITypeInfo( typeIndex ) );
    return vaSceneComponentRegistry::GetInstance( ).m_components[typeIndex].UITypeInfoCallback( );
}

bool Components::HasValidate( int typeIndex )
{
    assert( typeIndex >= 0 && typeIndex < TypeCount( ) );
    return vaSceneComponentRegistry::GetInstance( ).m_components[typeIndex].ValidateCallback != nullptr;
}

void Components::Validate( int typeIndex, entt::registry & registry, entt::entity entity )
{
    assert( HasValidate( typeIndex ) );
    return vaSceneComponentRegistry::GetInstance( ).m_components[typeIndex].ValidateCallback( registry, entity );
}

bool Components::HasListReferences( int typeIndex )
{
    assert( typeIndex >= 0 && typeIndex < TypeCount( ) );
    return vaSceneComponentRegistry::GetInstance( ).m_components[typeIndex].ListReferencesCallback != nullptr;
}

void Components::ListReferences( int typeIndex, entt::registry & registry, entt::entity entity, std::vector<Scene::EntityReference*> & referenceList )
{
    assert( HasValidate( typeIndex ) );
    return vaSceneComponentRegistry::GetInstance( ).m_components[typeIndex].ListReferencesCallback( registry, entity, referenceList );
}

bool Components::HasUIDraw( int typeIndex )
{
    assert( typeIndex >= 0 && typeIndex < TypeCount( ) );
    return vaSceneComponentRegistry::GetInstance( ).m_components[typeIndex].UIDrawCallback != nullptr;
}

void Components::UIDraw( int typeIndex, entt::registry & registry, entt::entity entity, vaDebugCanvas2D & canvas2D, vaDebugCanvas3D & canvas3D )
{
    assert( HasUIDraw( typeIndex ) );
    return vaSceneComponentRegistry::GetInstance( ).m_components[typeIndex].UIDrawCallback( registry, entity, canvas2D, canvas3D );
}

void Components::Reset( int typeIndex, entt::registry& registry, entt::entity entity )
{
    if( vaSceneComponentRegistry::GetInstance( ).m_components[typeIndex].ResetCallback != nullptr )
        vaSceneComponentRegistry::GetInstance( ).m_components[typeIndex].ResetCallback( registry, entity );
    else
    {
        // "dumb" reset - replace component with default constructed
        EmplaceOrReplace( typeIndex, registry, entity );
    }
}

vaSceneComponentRegistry::~vaSceneComponentRegistry( )
{

}

AccessPermissions::AccessPermissions( )
{
}

void AccessPermissions::SetState( State newState )
{
    assert( vaThreading::IsMainThread() );

    assert( newState != m_state );

    if( newState == AccessPermissions::State::SerializedDelete )
        { assert( m_state == AccessPermissions::State::Serialized ); }
    if( newState == AccessPermissions::State::Serialized )
        { assert( m_state == AccessPermissions::State::SerializedDelete || m_state == AccessPermissions::State::Concurrent ); }
    if( newState == AccessPermissions::State::Concurrent )
        { assert( m_state == AccessPermissions::State::Serialized ); }

    if( newState == AccessPermissions::State::Concurrent || m_state == AccessPermissions::State::Concurrent )
    {
#ifdef _DEBUG
        for( int lock : m_locks )
            { assert( lock == 0 ); };
#endif
    }

    if( m_locks.size() != Components::TypeCount( ) )
        m_locks.resize( Components::TypeCount( ), 0 );

    m_state = newState;
}

bool AccessPermissions::TryAcquire( const std::vector<int> & readWriteComponents, const std::vector<int> & readComponents )
{
    for( int i = 0; i < readWriteComponents.size( ); i++ )
    {
        int typeIndex = readWriteComponents[i];
        if( m_locks[typeIndex] != 0 )
        {
            if( m_locks[typeIndex] == -1 )
                VA_ERROR( "  Can't read-write lock component '%s' because it's already locked for read-write", Scene::Components::TypeName( typeIndex ).c_str() );
            else
                VA_ERROR( "  Can't read-write lock component '%s' because it's already locked for read", Scene::Components::TypeName( typeIndex ).c_str() );
            
            // unroll read-write
            for( int j = i-1; j >= 0; j-- )
                m_locks[readComponents[j]] = 0;

            return false;
        }
        m_locks[typeIndex] = -1;
    }

    for( int i = 0; i < readComponents.size( ); i++ )
    {
        int typeIndex = readComponents[i];
        if( m_locks[typeIndex] == -1 )
        {
            VA_ERROR( "  Can't read-only lock component '%s' because it's already locked for read-write", Scene::Components::TypeName( typeIndex ).c_str( ) );

            // unroll read-only
            for( int j = i - 1; j >= 0; j-- )
                m_locks[readComponents[j]]--;
            // unroll read-write
            for( int j = 0; j < readWriteComponents.size( ); j++ )
                m_locks[readWriteComponents[j]] = 0;

            return false;
        }
        m_locks[typeIndex]++;
    }
    return true;
}

void AccessPermissions::Release( const std::vector<int> & readWriteComponents, const std::vector<int>& readComponents )
{
    for( int i = 0; i < readWriteComponents.size( ); i++ )
    {
        assert( m_locks[readWriteComponents[i]] == -1 );
        m_locks[readWriteComponents[i]] = 0;
    }

    for( int i = 0; i < readComponents.size( ); i++ )
    {
        assert( m_locks[readComponents[i]] > 0 );
        m_locks[readComponents[i]]--;
    }
}

UIDRegistry::UIDRegistry( entt::registry & registry ) : m_registry( registry )
{
    m_registry.on_destroy<Scene::UID>( ).connect<&UIDRegistry::OnDestroy>( this );
    m_registry.on_update<Scene::UID>( ).connect<&UIDRegistry::OnDisallowedOperation>( this );
    m_registry.on_construct<Scene::UID>( ).connect<&UIDRegistry::OnEmplace>( this );
}
UIDRegistry::~UIDRegistry( )
{
    m_registry.on_destroy<Scene::UID>( ).disconnect<&UIDRegistry::OnDestroy>( this );
    m_registry.on_update<Scene::UID>( ).disconnect<&UIDRegistry::OnDisallowedOperation>( this );
    m_registry.on_construct<Scene::UID>( ).disconnect<&UIDRegistry::OnEmplace>( this );
    assert( m_UIDMap.empty() );
}

bool EntityReference::Serialize( SerializeArgs & args, vaSerializer & serializer, const string & key )
{
    UID serializationUID = vaGUID::Null;
    if( serializer.IsReading() )
    {
        m_entity = entt::null;
        if( serializer.Serialize<vaGUID>( key, serializationUID ) && (serializationUID != vaGUID::Null) )
        {
            args.LoadedReferences.push_back( {this, serializationUID} );
            return true;
        }
        return true;
    } else if( serializer.IsWriting() )
    {
        if( m_entity != entt::null && !args.UIDRegistry.Registry().valid(m_entity) )
        {
            m_entity = entt::null;
            VA_LOG( "EntityReference::Serialize - found a non-valid reference, resetting to entt::null" );
        }

        // we could avoid writing anything if the entity is not null, but let's write NULL GUID instead so it's more human-readable (TODO: revise later)
        if( m_entity != entt::null )
        {
            const Scene::UID * uid = args.UIDRegistry.Registry().try_get<Scene::UID>( m_entity );
            if( uid == nullptr )
            {
                assert( false ); // all referenced entities must have Scene::UID component; this can be created with UIDRegistry::GetOrCreate( entity )
                return false;
            }
            serializationUID = *uid;
        }
        return serializer.Serialize<vaGUID>( key, (vaGUID&)serializationUID );
    }
    assert( false ); return false;
}

void UIDRegistry::OnDisallowedOperation( entt::registry & registry, entt::entity )
{
    assert( &registry == &m_registry ); registry;
    assert( false );    // you're doing something that's not allowed
}

void UIDRegistry::OnDestroy( entt::registry & registry, entt::entity entity )
{
    assert( vaThreading::IsMainThread( ) && m_registry.ctx<Scene::AccessPermissions>().GetState( ) != Scene::AccessPermissions::State::Concurrent );
    assert( &registry == &m_registry );
    // remove from the map
    const Scene::UID & uid = registry.get<Scene::UID>( entity );
    size_t erasedCount = m_UIDMap.erase( uid );
    assert( erasedCount == 1 ); erasedCount;
}

void UIDRegistry::OnEmplace( entt::registry & registry, entt::entity entity )
{
    assert( vaThreading::IsMainThread( ) && m_registry.ctx<Scene::AccessPermissions>().GetState( ) != Scene::AccessPermissions::State::Concurrent );
    assert( &registry == &m_registry );
    // add to the map
    const Scene::UID & uid = registry.get<Scene::UID>( entity );
    auto ret = m_UIDMap.insert( {uid, entity} );
    assert( ret.second ); // messing with Scene::UID manually is disallowed and will cause these
}

Scene::UID UIDRegistry::GetOrCreate( entt::entity entity )
{
    assert( vaThreading::IsMainThread( ) && m_registry.ctx<Scene::AccessPermissions>().GetState( ) != Scene::AccessPermissions::State::Concurrent );
    assert( m_registry.valid( entity ) );
    const Scene::UID * uid = m_registry.try_get<Scene::UID>( entity );
    if( uid == nullptr )
    {
        Scene::UID newUID = vaGUID::Create( );
        m_registry.emplace<Scene::UID>( entity, newUID );
        return newUID;
    }
    return *uid;
}

entt::entity UIDRegistry::Find( const Scene::UID & uid ) const
{
    auto it = m_UIDMap.find( uid );
    if( it == m_UIDMap.end() )
        return entt::null;
    else
        return it->second;
}
//
//entt::entity vaScene::UIDToEntity( const Scene::UID & serializationID )
//{
//    assert( vaThreading::IsMainThread( ) && m_registry.ctx<Scene::AccessPermissions>().GetState( ) != Scene::AccessPermissions::State::Concurrent );
//    return Find( serializationID );
//}
//
//Scene::UID vaScene::EntityToUID( entt::entity entity )
//{
//    assert( vaThreading::IsMainThread( ) && m_registry.ctx<Scene::AccessPermissions>().GetState( ) != Scene::AccessPermissions::State::Concurrent );
//    const Scene::UID * uid = m_registry.try_get<Scene::UID>( entity );
//    if( uid == nullptr )
//    {
//        Scene::UID newUID = vaGUID::Create( );
//        m_registry.emplace<Scene::UID>( entity, newUID );
//        return newUID;
//    }
//    else
//        return *uid;
//}

vaSceneComponentRegistry::vaSceneComponentRegistry( )
{
    // No need to register components if they don't need to get serialized, visible in the UI, accessed dynamically or etc.

    //int a = Components::RuntimeID<Relationship>( );
    RegisterComponent< DestroyTag > ( );
    RegisterComponent< UID >( "UID" );
    RegisterComponent< Name >( "Name" );            // <- just an example on how you can use a custom name - perhaps you want to shorten the name to something more readable or even substitute a component for another
    RegisterComponent< Relationship >( );
    RegisterComponent< TransformLocalIsWorldTag >( );
    RegisterComponent< TransformDirtyTag >( );
    RegisterComponent< TransformLocal >( );
    RegisterComponent< TransformWorld >( );
    RegisterComponent< PreviousTransformWorld >( );
    RegisterComponent< WorldBounds >( );
    RegisterComponent< RenderMesh >( );
    RegisterComponent< LocalIBLProbe >( );
    RegisterComponent< DistantIBLProbe >( );
    RegisterComponent< FogSphere >( );
    RegisterComponent< CustomBoundingBox >( );
    RegisterComponent< WorldBoundsDirtyTag >( );
    RegisterComponent< LightAmbient >( );
    //RegisterComponent< LightDirectional >( );
    RegisterComponent< LightPoint >( );
    RegisterComponent< EmissiveMaterialDriver >( );
    RegisterComponent< SkyboxTexture >( );
    RegisterComponent< IgnoreByIBLTag >( );
    RegisterComponent< RenderCamera >( );
}

