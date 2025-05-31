ProjectileId = {SHELL = 0, CRUISE_MISSILE = 1, MISSILE = 2, TORPEDO = 3}
ProjectileClass = {SHELL = 0, CRUISE_MISSILE = 1, MISSILE = 2, TORPEDO = 3}
G = 9.8

projectiles = {
	{
		projectileClass = ProjectileClass.SHELL,
		size = {x = 1, y = 1, z = 1},
		speed = 5,
		rotAngle = .1,
		explosion = {damage = 100, radius = 20, sfx = PATH .. 'Sounds/SFX/Explosions/explosion02.ogg'},
		directHitDamage = 200,
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
		explosion = {damage = 100, radius = 20, sfx = PATH .. 'Sounds/SFX/Explosions/explosion02.ogg'},
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
		explosion = {damage = 70, radius = 12, sfx = PATH .. 'Sounds/SFX/Explosions/explosion02.ogg'},
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
		explosion = {damage = 20, radius = 3, sfx = PATH .. 'Sounds/Units/Submarines/torpedo.ogg'},
		directHitDamage = 450,
		rayLength = 3.5,
		name = "Torpedo",
	    basePath = PATH .. "Models/GameObjects/Projectiles/Torpedos/",
		meshPath = "torpedo.xml",
		albedoPath = 'torpedo.jpg'
	}
}
