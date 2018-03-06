/**
* Author ZhongLou Yang, 2018-3-6
*/
#pragma once

namespace Concise
{
	class FileUtils
	{
	public:
		static FileUtils & Instance();
	private:
		FileUtils();
		~FileUtils();
	public:
		/** TODO */
		const std::string GetAssetsPath() const 
		{
#if defined(ASSETS_DIR)
			return ASSETS_DIR;
#else
#error("[Error], Currently must define ASSETS_DIR explictly.")
#endif
		}
	};
}