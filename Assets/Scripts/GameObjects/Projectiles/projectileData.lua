ProjectileId = {HE_SHELL = 0, EMP_SHELL = 1, CRUISE_MISSILE = 2, MISSILE = 3, TORPEDO = 4}
ProjectileClass = {SHELL = 0, CRUISE_MISSILE = 1, MISSILE = 2, TORPEDO = 3}
G = 9.8

explosionVfx = {
	vfx = true,
	duration = 50,
	offset = 0,
	mesh = {
		numParticles = 1,
		texture = PATH .. 'Textures/Explosion/explosion07.png',
		lowLife = 3,
		highLife = 3,
		size = {x = 10, y = 10},
	}
}
explosionSfx = {
	vfx = false,
	offset = 100,
	path = PATH .. 'Sounds/SFX/Explosions/explosion02.ogg',
	duration = 2500
}

projectiles = {
	{
		projectileClass = ProjectileClass.SHELL,
		size = {x = 1, y = 1, z = 1},
		speed = 5,
		rotAngle = .1,
		explosion = {damage = 100, radius = 20, fx = {explosionVfx, explosionSfx}},
		directHitDamage = 200,
		rayLength = 1,
		name = "Shell",
	    basePath = PATH .. "Models/GameObjects/Projectiles/Shells/",
		meshPath = "shell.xml",
		albedoPath = 'shell.jpg'
	},
	{
		projectileClass = ProjectileClass.SHELL,
		size = {x = 1, y = 1, z = 1},
		speed = 5,
		rotAngle = .1,
		explosion = {damage = 0, radius = 20, fx = {explosionVfx, explosionSfx}},
		directHitDamage = 0,
		rayLength = 1,
		name = "Shell",
	    basePath = PATH .. "Models/GameObjects/Projectiles/Shells/",
		meshPath = "shell.xml",
		albedoPath = 'shell.jpg'
	},
	{
		projectileClass = ProjectileClass.CRUISE_MISSILE,
		size = {x = 6, y = 13.5, z = 2.54},
		speed = .5,
		rotAngle = .1,
		explosion = {damage = 100, radius = 20, fx = {explosionVfx, explosionSfx}},
		directHitDamage = 450,
		rayLength = 6,
		name = "Cruise missile",
	    basePath = PATH .. "Models/GameObjects/Projectiles/Missiles/",
		meshPath = "cruiseMissile.xml",
		albedoPath = 'cruiseMissile.jpg'
	},
	{
		projectileClass = ProjectileClass.MISSILE,
		size = {x = 3, y = 7., z = 1.27},
		speed = .5,
		rotAngle = .1,
		explosion = {damage = 70, radius = 12, fx = {explosionVfx, explosionSfx}},
		directHitDamage = 250,
		rayLength = 3,
		name = "Missile",
	    basePath = PATH .. "Models/GameObjects/Projectiles/Missiles/",
		meshPath = "missile.xml",
		albedoPath = 'cruiseMissile.jpg'
	},
	{
		projectileClass = ProjectileClass.TORPEDO,
		size = {x = 1, y = 6, z = 1},
		speed = .06,
		explosion = {damage = 20, radius = 3, fx = {explosionVfx, explosionSfx}},
		directHitDamage = 450,
		rayLength = 3.5,
		name = "Torpedo",
	    basePath = PATH .. "Models/GameObjects/Projectiles/Torpedos/",
		meshPath = "torpedo.xml",
		albedoPath = 'torpedo.jpg'
	}
}
