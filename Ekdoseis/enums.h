
#pragma once
enum ReturnFlag {
	CREATE_SUCCESS,
	CREATE_FAILURE,
	ALREADY_EXISTS,
	OVERRIDE_FAILURE,
	OVERRIDE_SUCCESS,
};
enum CreateFlag {
	NO_OVERRIDE,//default
	OVERRIDE_IF_EXISTS,
};
enum DoseCommand {
	INIT,
	ADD,
	COMMIT,
	STATUS,
	LOG,
	CHECKOUT,
	RESTORE,
	BRANCH,
	NOCOMMAND,

};


namespace index {
	enum class IndexFileStatus {
		COMMITTED = 1,// can be in index
		STAGED = 2,//can be in index
	};
	enum class FileStatus {
		COMMITTED,// can be in index
		STAGED,//can be in index
		MODIFIED,
		UNTRACKED,
	};

	struct indexEntry {
		unsigned int createdTime;
		unsigned int modifiedTime;
		unsigned int sd_dev;//metadata
		unsigned int sd_ino;//metadata
		unsigned int mode;//permissoins -only this is used in windows
		unsigned int sd_uid;//metadata
		unsigned int sd_gid;//metadata
		unsigned int sd_size;//filesize
		std::string   sha1;//what is signed and unsigned char 
		unsigned int flag;
		std::string fileName;//variable length null terminated

	};
};

