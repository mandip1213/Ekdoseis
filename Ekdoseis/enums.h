
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
	NOCOMMAND,

};
