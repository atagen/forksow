#include <algorithm>

#include "qcommon/base.h"
#include "client/assets.h"
#include "cgame/cg_local.h"

struct Spray {
	Vec3 origin;
	Vec3 normal;
	float radius;
	float angle;
	StringHash material;
	s64 spawn_time;
};

static StringHash spray_assets[ 4096 ];
static size_t num_spray_assets;

constexpr static s64 SPRAY_LIFETIME = 60000;

static Spray sprays[ 1024 ];
static size_t sprays_head;
static size_t num_sprays;

void InitSprays() {
	num_spray_assets = 0;

	for( const char * path : AssetPaths() ) {
		Span< const char > ext = FileExtension( path );
		if( StartsWith( path, "textures/sprays/" ) && ( ext == ".png" || ext == ".jpg" || ext == ".dds" ) ) {
			assert( num_spray_assets < ARRAY_COUNT( spray_assets ) );

			spray_assets[ num_spray_assets ] = StringHash( StripExtension( path ) );
			num_spray_assets++;
		}
	}

	std::sort( spray_assets, spray_assets + num_spray_assets, []( StringHash a, StringHash b ) {
		return a.hash < b.hash;
	} );

	sprays_head = 0;
	num_sprays = 0;
}

void AddSpray( Vec3 origin, Vec3 normal, Vec3 angles, u64 entropy ) {
	RNG rng = NewRNG( entropy, 0 );

	Vec3 forward, up;
	AngleVectors( angles, &forward, NULL, &up );

	Spray spray;
	spray.origin = origin;
	spray.normal = normal;
	spray.material = num_spray_assets == 0 ? StringHash( "" ) : RandomElement( &rng, spray_assets, num_spray_assets );
	spray.radius = RandomUniformFloat( &rng, 32.0f, 48.0f );
	spray.spawn_time = cls.gametime;

	Vec3 left = Cross( normal, up );
	Vec3 decal_up = Normalize( Cross( left, normal ) );

	Vec3 tangent, bitangent;
	OrthonormalBasis( normal, &tangent, &bitangent );

	spray.angle = -atan2f( Dot( decal_up, tangent ), Dot( decal_up, bitangent ) );
	spray.angle += RandomFloat11( &rng ) * Radians( 10.0f );

	sprays[ ( sprays_head + num_sprays ) % ARRAY_COUNT( sprays ) ] = spray;

	if( num_sprays == ARRAY_COUNT( sprays ) ) {
		sprays_head++;
	}
	else {
		num_sprays++;
	}

	DoVisualEffect( "vfx/spray", origin - forward * 8.0f, forward );
}

void DrawSprays() {
	while( num_sprays > 0 ) {
		if( sprays[ sprays_head % ARRAY_COUNT( sprays ) ].spawn_time + SPRAY_LIFETIME >= cls.gametime )
			break;
		sprays_head++;
		num_sprays--;
	}

	for( size_t i = 0; i < num_sprays; i++ ) {
		const Spray * spray = &sprays[ ( sprays_head + i ) % ARRAY_COUNT( sprays ) ];
		DrawDecal( spray->origin, spray->normal, spray->radius, spray->angle, spray->material, vec4_white, 2.0f );
	}
}
