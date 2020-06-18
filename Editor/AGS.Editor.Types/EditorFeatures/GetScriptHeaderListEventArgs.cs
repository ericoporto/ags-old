using System.Collections.Generic;

namespace AGS.Editor.Types
{
	public class GetScriptHeaderListEventArgs
	{
		IList<Script> _scripts;

		public GetScriptHeaderListEventArgs(IList<Script> scriptHeaders)
		{
			_scripts = scriptHeaders;
		}

		public IList<Script> ScriptHeaders
		{
			get { return _scripts; }
		}
	}
}
