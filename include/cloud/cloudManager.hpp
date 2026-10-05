#ifndef CLOUDMANAGER_HPP
# define CLOUDMANAGER_HPP

# include <string>

class CloudManager
{
	private:
		static std::string	parseMultipartNameFile_(const std::string& postBody);
		static std::string	parsePureFileContent(const std::string& postBody);

		static bool			saveFileOnDisk_(const std::string& filePath, const std::string& fileContent);
		static bool			deleteFileOnDisk_(const std::string& filePath);


	public:
		static bool	processUpload(const std::string& postBody, const std::string& saveDirectory);
		static bool	processDelete(std::string targetPath, const std::string& saveDirectory);
};

#endif