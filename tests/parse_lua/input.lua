local data =
{
	serial = 
	{
		"lua",
		"xml"
	},
	include =
	{
		{
			external = false,
			name = "CppGen.h"
		},
		{
			external = true,
			name = "string"
		}
	},
	define =
	{
		building = 
		{
			price =
			{
				type = "int",
				value = 1000
			},
			texture =
			{
				type = "std::string"
			},
			height =
			{
				type = "int",
				value = 16
			}
		},
		person =
		{
			name =
			{
				type = "std::string",
				value = "peter"
			},
			age =
			{
				type = "int"
			}
		},
		your_mom =
		{
			weight =
			{
				type = "int",
				value = 10000000
			}
		}
	}
}
return data