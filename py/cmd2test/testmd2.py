
import traceback
import os
import pathlib
import cmd2

class TestCmd2(cmd2.Cmd):
    def __init__(self):
        super().__init__()

        self.self_in_py = True
        self.timing = True

        self.teststr = "DID THIS WORK"
        self.cmd_hist_metadata = []

         # register three hooks
        self.register_postparsing_hook(self._cmd_hist_processor)

    @staticmethod
    def dumpobj(obj):
        for attr in dir(obj):
            print("obj.%s = %r" % (attr, getattr(obj, attr)))

    def _cmd_hist_processor(self, data: cmd2.plugin.PostparsingData) -> cmd2.plugin.PostparsingData:

        self.cmd_hist_metadata.append({
            "arg_list": data.statement.arg_list,
            "args": data.statement.args,
            "argv": data.statement.argv,
            "command": data.statement.command,
            "command_and_args": data.statement.command_and_args,
            "expanded_command_line": data.statement.expanded_command_line,
            "raw": data.statement.raw,
        })

        return data

    def do_hello(self, _):
        print("hello")

    def do_getmystr(self,_):
        self.last_result = self.teststr

    def do_toggletimer(self, _):        
        self.timing =  not self.timing

    @cmd2.with_argument_list
    def do_dirlist(self, arglist):
        """
        usage: dirlist <directory>
        """

        # Expect 1 argument, the directory to change to
        if not arglist or len(arglist) != 1:
            self.perror("cd requires exactly 1 argument:")
            self.do_help('dirlist')
            self.last_result = 'Bad arguments'
            return
        try:
            if os.path.isdir(arglist[0]):
                self.last_result = os.listdir(pathlib.Path(arglist[0]).resolve())
                print(self.last_result)
            else:
                self.last_result = "Bad Param {}".format(arglist[0])
        except Exception as e:
            print(e)
            traceback.print_exc()
    
    def do_cmd_meta(self,_):
        try:
            i = 1
            for entry in self.cmd_hist_metadata:
                print(f'({i}) \"{entry["raw"]} --> {entry["command_and_args"]}\":')
                i+=1
                for k,v in entry.items():
                    print(f'\t{k}:{v}')
        except Exception as e:
            print(e)
            traceback.print_exc()


if __name__ == '__main__':
    import sys
    c = TestCmd2()
    sys.exit(c.cmdloop())


